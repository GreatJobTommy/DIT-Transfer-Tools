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
@click.option(
    "--rsync-fallback",
    is_flag=True,
    help="Use rsync --whole-file -a fallback for full metadata/attributes (auto for LTFS)",
)
@click.option(
    "--spot-check-hashes",
    is_flag=True,
    help="Spot-check hashes (size + N random chunks, auto for LTFS verify)",
)
@click.option("--num-chunks", type=int, default=5, help="Number of spot-check chunks")
@click.option("--chunk-mb", type=int, default=1, help="Spot-check chunk size MB")
def transfer(
    source,
    dest,
    verify,
    password,
    key_file,
    concurrency,
    rsync_fallback,
    spot_check_hashes,
    num_chunks,
    chunk_mb,
):
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
        if is_sftp_source and is_sftp_dest:
            raise NotImplementedError("Direct SFTP to SFTP transfer not supported yet.")
        elif (is_sftp_source or is_sftp_dest) and (is_rclone_source or is_rclone_dest):
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
        elif is_rclone_source or is_rclone_dest:
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
            else:
                user, host, port, remote_dest = parse_sftp_uri(dest)
                sftp, client = sftp_connect(user, host, port, password, key_file)
                transfer_local_to_sftp(src_path, remote_dest, sftp, verify)
        else:
            transfer_local(
                src_path,
                dst_path,
                verify=verify,
                rsync_fallback=rsync_fallback,
                spot_check=spot_check_hashes,
                num_chunks=num_chunks,
                chunk_mb=chunk_mb,
            )
        click.echo("Transfer completed successfully.")
    except Exception as e:
        click.echo(f"Error: {e}", err=True)
        raise
    finally:
        if client:
            client.close()


@cli.command()
@click.argument("src")
@click.argument("dest")
@click.option(
    "--dry-run",
    is_flag=True,
    default=True,
    help="Dry run mode (default). No changes made.",
)
def verify(src, dest, dry_run):
    """Verify directories by file sizes (local only)."""
    src_path = Path(src)
    dest_path = Path(dest)
    if not src_path.is_dir():
        click.echo(click.style(f"Error: {src} is not a directory", fg="red"), err=True)
        sys.exit(1)
    if not dest_path.is_dir():
        click.echo(click.style(f"Error: {dest} is not a directory", fg="red"), err=True)
        sys.exit(1)

    def get_sizes(root_path):
        sizes = {}
        for r, dirs, files in os.walk(root_path):
            rel_root = os.path.relpath(r, str(root_path))
            for f in files:
                fpath = os.path.join(r, f)
                try:
                    st = os.stat(fpath)
                    rel = os.path.join(rel_root, f) if rel_root != "." else f
                    sizes[rel] = st.st_size
                except OSError as e:
                    click.echo(
                        click.style(f"Warning: cannot stat {fpath}: {e}", fg="yellow"),
                        err=True,
                    )
        return sizes

    src_sizes = get_sizes(src_path)
    dest_sizes = get_sizes(dest_path)

    src_files = set(src_sizes.keys())
    dest_files = set(dest_sizes.keys())

    missing = sorted(src_files - dest_files)
    extra = sorted(dest_files - src_files)
    mismatches = [
        f
        for f in sorted(src_files.intersection(dest_files))
        if src_sizes[f] != dest_sizes[f]
    ]

    has_issues = bool(missing or extra or mismatches)

    if missing:
        click.echo(click.style("Missing files in DEST:", fg="red"))
        for f in missing:
            click.echo(f"  - {f}")

    if extra:
        click.echo(click.style("Extra files in DEST:", fg="red"))
        for f in extra:
            click.echo(f"  - {f}")

    if mismatches:
        click.echo(click.style("Size mismatches:", fg="yellow"))
        for f in mismatches:
            click.echo(
                f"  - {f} (SRC: {src_sizes[f]} bytes, DEST: {dest_sizes[f]} bytes)"
            )

    if not has_issues:
        click.echo(click.style("✓ All files match by size.", fg="green"))
    else:
        click.echo("(dry-run: no actions taken)" if dry_run else "Verification failed.")

    if has_issues:
        sys.exit(1)


if __name__ == "__main__":
    cli()
