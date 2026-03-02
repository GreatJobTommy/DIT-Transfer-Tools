import click
from pathlib import Path
import os
from .transfer import (
    parse_sftp_uri,
    sftp_connect,
    transfer_local_to_sftp,
    transfer_sftp_to_local,
    transfer_local,
    parse_rclone_uri,
    ensure_rclone_remote,
    transfer_with_rclone,
)

@click.command()
@click.argument("source")
@click.argument("dest")
@click.option("--verify", is_flag=True, help="Verify checksums after transfer")
@click.option("--password", help="SFTP password (prompts if not provided)")
@click.option("--key-file", type=click.Path(), help="Path to private key file")
@click.option("--concurrency", type=int, default=4, help="Number of parallel transfers for rclone (default: 4)")
def main(source, dest, verify, password, key_file, concurrency):
    """Transfer files or directories from SOURCE to DEST."""
    src_path = Path(source)
    dst_path = Path(dest)
    is_sftp_source = source.startswith("sftp://")
    is_sftp_dest = dest.startswith("sftp://")
    is_rclone_source = source.startswith("rclone://")
    is_rclone_dest = dest.startswith("rclone://")
    client = None
    try:
        if (is_sftp_source or is_sftp_dest) and (is_rclone_source or is_rclone_dest):
            raise NotImplementedError("Mixed SFTP and rclone transfers not supported yet.")
        if is_sftp_source and is_sftp_dest:
            raise NotImplementedError("Direct SFTP to SFTP transfer not supported yet.")
        if is_rclone_source or is_rclone_dest:
            if is_rclone_source:
                remote_name, host, user, passwd, port, remote_path = parse_rclone_uri(source)
                rclone_source_path = f"{remote_name}:{remote_path}"
                if user or passwd:
                    ensure_rclone_remote(remote_name, user or os.getenv('USER'), passwd, host, port)
            else:
                rclone_source_path = str(src_path)
            if is_rclone_dest:
                remote_name, host, user, passwd, port, remote_path = parse_rclone_uri(dest)
                rclone_dest_path = f"{remote_name}:{remote_path}"
                if user or passwd:
                    ensure_rclone_remote(remote_name, user or os.getenv('USER'), passwd, host, port)
            else:
                rclone_dest_path = str(dst_path)
            transfer_with_rclone(rclone_source_path, rclone_dest_path, verify, concurrency)
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

if __name__ == "__main__":
    main()