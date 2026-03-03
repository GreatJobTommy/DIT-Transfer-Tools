import click
import os
import sys
from pathlib import Path
from .transfer import (
    parse_sftp_uri,
    sftp_connect,
    transfer_local_to_sftp,
    transfer_sftp_to_local,
    transfer_local,
    parse_rclone_uri,
    ensure_rclone_remote,
    transfer_with_rclone,
    create_temp_rclone_sftp_remote,
    cleanup_temp_rclone_remote,
)

import hashlib
import json
import random
import stat
import subprocess


@click.group()
def cli():
    """DIT Transfer Tools CLI."""
    pass


@cli.command()
@click.argument("source")
@click.argument("dest")
@click.option("--verify", is_flag=True, help="Verify checksums after transfer")
@click.option("--password", help="SFTP password (prompts if not provided)")
@click.option("--key-file", type=click.Path(), help="Path to private key file")
@click.option(
    "--concurrency",
    type=int,
    default=4,
    help="Number of parallel transfers for rclone (default: 4)",
)
def transfer(source, dest, verify, password, key_file, concurrency):
    """Transfer files or directories from SOURCE to DEST.
    Supports local (incl. LTFS mounts like /Volumes/LTO*), sftp://, rclone://."""
    src_path = Path(source)
    dst_path = Path(dest)
    is_sftp_source = source.startswith("sftp://")
    is_sftp_dest = dest.startswith("sftp://")
    is_rclone_source = source.startswith("rclone://")
    is_rclone_dest = dest.startswith("rclone://")
    client = None
    try:
        if (is_sftp_source or is_sftp_dest) and (is_rclone_source or is_rclone_dest):
            password = password or click.prompt("Enter SFTP password", hide_input=True)
            if is_sftp_source:
                rem_s, path_s = create_temp_rclone_sftp_remote(source, password)
                r_src = f"{rem_s}:{path_s}"
                rem_d_name, _, _, _, _, path_d = parse_rclone_uri(dest)
                r_dst = f"{rem_d_name}:{path_d}"
                transfer_with_rclone(r_src, r_dst, verify, concurrency)
                cleanup_temp_rclone_remote(rem_s)
            else:
                rem_s_name, _, _, _, _, path_s = parse_rclone_uri(source)
                r_src = f"{rem_s_name}:{path_s}"
                rem_d, path_d = create_temp_rclone_sftp_remote(dest, password)
                r_dst = f"{rem_d}:{path_d}"
                transfer_with_rclone(r_src, r_dst, verify, concurrency)
                cleanup_temp_rclone_remote(rem_d)
            click.echo("Transfer completed successfully.")
        if is_sftp_source and is_sftp_dest:
            raise NotImplementedError("Direct SFTP to SFTP transfer not supported yet.")
        if is_rclone_source or is_rclone_dest:
            if is_rclone_source:
                remote_name, host, user, passwd, port, remote_path = parse_rclone_uri(
                    source
                )
                rclone_source_path = f"{remote_name}:{remote_path}"
                if user or passwd:
                    ensure_rclone_remote(
                        remote_name, user or os.getenv("USER"), passwd, host, port
                    )
            else:
                rclone_source_path = str(src_path)
            if is_rclone_dest:
                remote_name, host, user, passwd, port, remote_path = parse_rclone_uri(
                    dest
                )
                rclone_dest_path = f"{remote_name}:{remote_path}"
                if user or passwd:
                    ensure_rclone_remote(
                        remote_name, user or os.getenv("USER"), passwd, host, port
                    )
            else:
                rclone_dest_path = str(dst_path)
            transfer_with_rclone(
                rclone_source_path, rclone_dest_path, verify, concurrency
            )
        elif is_sftp_source or is_sftp_dest:
            password = password or click.prompt("Enter SFTP password", hide_input=True)
            if is_sftp_source:
                user, host, port, remote_src = parse_sftp_uri(source)
                sftp, client = sftp_connect(user, host, port, password, key_file)
                transfer_sftp_to_local(remote_src, dst_path, sftp, verify)
            elif is_sftp_dest:
                user, host, port, remote_dest = parse_sftp_uri(dest)
                sftp, client = sftp_connect(user, host, port, password, key_file)
                transfer_local_to_sftp(src_path, remote_dest, sftp, verify)
        else:
            transfer_local(src_path, dst_path, verify)
        click.echo("Transfer completed successfully.")
    except Exception as e:
        click.echo(f"Error: {e}", err=True)
        raise
    finally:
        if client:
            client.close()


def parse_location(uri: str) -> dict[str, any]:
    \"\"\"Parse location URI.\"\"\"

    if uri.startswith("sftp://"):
        loc = {"type": "sftp", "uri": uri}
    elif uri.startswith("rclone://"):
        remote_name, host, user, passwd, port, path = parse_rclone_uri(uri)
        loc = {"type": "rclone", "remote_name": remote_name, "remote_path": path, "host": host, "user": user, "passwd": passwd, "port": port}
    else:
        loc = {"type": "local", "path": Path(uri)}
    return loc


def walk_location(loc: dict, password: str = None) -> tuple[dict[str, int], tuple or None]:
    sizes: dict[str, int] = {}
    client_tuple = None
    if loc["type"] == "local":
        path = loc["path"]
        if not path.is_dir():
            raise click.BadParameter(f"{path} is not a directory", param_hint="src/dest")
        for r, dirs, files in os.walk(path):
            rel_root = os.path.relpath(r, str(path))
            for f in files:
                fpath = os.path.join(r, f)
                try:
                    sizes[os.path.join(rel_root, f) if rel_root != "." else f] = os.stat(fpath).st_size
                except OSError as e:
                    click.echo(click.style(f"Warning: cannot stat {fpath}: {e}", fg="yellow"), err=True)
    elif loc["type"] == "sftp":
        user, host, port, remote_path = parse_sftp_uri(loc["uri"])
        passw = password or click.prompt(f"Enter SFTP password for {host}:", hide_input=True)
        sftp, ssh_client = sftp_connect(user, host, port, passw)
        client_tuple = (sftp, ssh_client)
        def rec_list(rpath: str) -> dict[str, int]:
            sub_sizes = {}
            try:
                attrs = sftp.listdir_attr(rpath or "/")
            except FileNotFoundError:
                return sub_sizes
            base_len = len(remote_path.rstrip("/"))
            for attr in attrs:
                if attr.filename in {".", ".."}:
                    continue
                fpath = (rpath.rstrip("/") + "/" + attr.filename) if rpath else attr.filename
                relpath = fpath[base_len + 1 :] if base_len < len(fpath) else fpath
                if stat.S_ISDIR(attr.st_mode):
                    sub_sizes.update(rec_list(fpath))
                elif stat.S_ISREG(attr.st_mode):
                    sub_sizes[relpath] = attr.st_size
            return sub_sizes
        sizes = rec_list(remote_path)
    elif loc["type"] == "rclone":
        remote_name = loc["remote_name"]
        host = loc["host"]
        user = loc["user"] or os.getenv("USER")
        passwd = loc["passwd"]
        port = loc["port"]
        remote_path = loc["remote_path"]
        if passwd:
            ensure_rclone_remote(remote_name, user, passwd, host, port)
        cmd = ["rclone", "lsjson", f"{remote_name}:{remote_path}", "--recursive", "--files-only"]
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        files = []
        for line in result.stdout.strip().split("\n"):
            if line.strip():
                files.append(json.loads(line))
        base_len = len(remote_path.rstrip("/"))
        for f in files:
            path = f["Path"]
            rel = path[base_len + 1 :] if base_len < len(path) else path
            sizes[rel] = f["Size"]
    return sizes, client_tuple


def get_chunk_bytes(loc: dict, rel: str, offset: int, length: int, sftp_client=None) -> bytes:
    if loc["type"] == "local":
        path = loc["path"] / rel
        with open(path, "rb") as f:
            f.seek(offset)
            return f.read(length)
    elif loc["type"] == "sftp":
        remote_base = loc["remote_path"].rstrip("/") + "/" + rel
        with sftp_client.open(remote_base, "rb") as f:
            f.seek(offset)
            return f.read(length)
    elif loc["type"] == "rclone":
        rpath = f"{loc['remote_name']}:{loc['remote_path'].rstrip('/')}/{rel}"
        cmd = ["rclone", "cat", rpath, "--offset", str(offset), "--count", str(length)]
        result = subprocess.run(cmd, capture_output=True, check=True)
        return result.stdout


def generate_offsets(size: int, chunk_size: int, samples: int, sequential: bool) -> list[int]:
    num = min(samples, size // chunk_size)
    if sequential:
        return [i * chunk_size for i in range(num)]
    else:
        offsets = set()
        step = 65536
        while len(offsets) < num:
            off = random.randrange(0, size - chunk_size + 1, step)
            offsets.add(off)
        return sorted(offsets)


@cli.command()
@click.argument("src")
@click.argument("dest")
@click.option("--hash-algo", type=click.Choice(["none", "md5", "sha256"]), default="none", help="Hash algorithm for spot-checks")
@click.option("--samples", type=int, default=5, help="Number of random chunks to hash")
@click.option("--chunk-mb", type=int, default=1, help="Chunk size in MB")
@click.option("--sequential", is_flag=True, help="Use sequential chunks from start (good for LTO/tapes)")
@click.option("--json", "json_out", is_flag=True, help="Output JSON report")
@click.option("--dry-run", is_flag=True, help="Dry run (default True)")
@click.option("--password", help="SFTP password (overrides prompt)")
def verify(src, dest, hash_algo, samples, chunk_mb, sequential, json_out, dry_run, password):
    src_loc = parse_location(src)
    dest_loc = parse_location(dest)
    src_sizes, src_client = walk_location(src_loc, password)
    dest_sizes, dest_client = walk_location(dest_loc, password)
    src_files = set(src_sizes.keys())
    dest_files = set(dest_sizes.keys())
    missing = sorted(src_files - dest_files)
    extra = sorted(dest_files - src_files)
    size_mismatches = [f for f in sorted(src_files & dest_files) if src_sizes[f] != dest_sizes[f]]
    issues = {
        "summary": {
            "total_src": len(src_sizes),
            "total_dest": len(dest_sizes),
            "missing": len(missing),
            "extra": len(extra),
            "size_mismatches": len(size_mismatches),
            "hash_mismatches": 0
        },
        "missing": missing,
        "extra": extra,
        "size_mismatches": size_mismatches,
        "hash_mismatches": []
    }
    src_sftp = src_client[0] if src_client else None
    dest_sftp = dest_client[0] if dest_client else None
    if hash_algo != "none":
        chunk_size = chunk_mb * 1024 * 1024
        for f in sorted(src_sizes):
            if f not in dest_sizes or src_sizes[f] != dest_sizes[f]:
                continue
            size = src_sizes[f]
            if size < chunk_size:
                continue
            offsets = generate_offsets(size, chunk_size, samples, sequential)
            chunk_issues = []
            for off in offsets:
                try:
                    src_chunk = get_chunk_bytes(src_loc, f, off, chunk_size, src_sftp)
                    dest_chunk = get_chunk_bytes(dest_loc, f, off, chunk_size, dest_sftp)
                    src_h = hashlib.new(hash_algo).update(src_chunk).hexdigest()
                    dest_h = hashlib.new(hash_algo).update(dest_chunk).hexdigest()
                    if src_h != dest_h:
                        chunk_issues.append({
                            "offset": off,
                            "src_hash": src_h,
                            "dest_hash": dest_h
                        })
                except Exception as e:
                    chunk_issues.append({"offset": off, "error": str(e)})
            if chunk_issues:
                issues["hash_mismatches"].append({"file": f, "chunks": chunk_issues})
        issues["summary"]["hash_mismatches"] = len(issues["hash_mismatches"])
    if src_client:
        src_client[1].close()
    if dest_client:
        dest_client[1].close()
    has_issues = bool(issues["summary"]["missing"] or issues["summary"]["extra"] or issues["summary"]["size_mismatches"] or issues["summary"]["hash_mismatches"])
    if json_out:
        print(json.dumps(issues, indent=2))
    else:
        if dry_run:
            click.echo("(dry-run: no actions taken)")
        if not has_issues:
            click.echo(click.style("✓ Verification passed: all files match by size and hash.", fg="green"))
        else:
            click.echo(click.style("Verification issues found:", fg="red"))
            if missing:
                click.echo(click.style(f"Missing {len(missing)} files in DEST:", fg="red"))
                for f in missing[:10]:
                    click.echo(f"  - {f}")
                if len(missing) > 10:
                    click.echo(f"  ... and {len(missing)-10} more")
            if extra:
                click.echo(click.style(f"Extra {len(extra)} files in DEST:", fg="red"))
                for f in extra[:10]:
                    click.echo(f"  - {f}")
                if len(extra) > 10:
                    click.echo(f"  ... and {len(extra)-10} more")
            if size_mismatches:
                click.echo(click.style(f"Size mismatches {len(size_mismatches)}:", fg="yellow"))
                for f in size_mismatches[:10]:
                    click.echo(f"  - {f} (SRC: {src_sizes[f]}, DEST: {dest_sizes[f]})")
                if len(size_mismatches) > 10:
                    click.echo(f"  ... and {len(size_mismatches)-10} more")
            if issues["hash_mismatches"]:
                click.echo(click.style(f"Hash mismatches in {len(issues['hash_mismatches'])} files:", fg="red"))
                for hm in issues["hash_mismatches"][:5]:
                    click.echo(f"  File: {hm['file']}")
                    for c in hm['chunks'][:3]:
                        if 'error' in c:
                            click.echo(f"    offset {c['offset']}: ERROR {c['error']}")
                        else:
                            click.echo(f"    offset {c['offset']}: SRC {c['src_hash'][:8]}... != DEST {c['dest_hash'][:8]}...")
                    if len(hm['chunks']) > 3:
                        click.echo("    ... more")
                if len(issues["hash_mismatches"]) > 5:
                    click.echo(f"  ... and {len(issues['hash_mismatches'])-5} more files")
    sys.exit(0 if not has_issues else 1)


if __name__ == "__main__":
    cli()
