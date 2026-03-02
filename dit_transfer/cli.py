import click
from pathlib import Path
from .transfer import (
    parse_sftp_uri,
    sftp_connect,
    transfer_local_to_sftp,
    transfer_sftp_to_local,
    transfer_local,
)
@click.command()
@click.argument(&quot;source&quot;)
@click.argument(&quot;dest&quot;)
@click.option(&quot;--verify&quot;, is_flag=True, help=&quot;Verify checksums after transfer&quot;)
@click.option(&quot;--password&quot;, help=&quot;SFTP password (prompts if not provided)&quot;)
@click.option(&quot;--key-file&quot;, type=click.Path(), help=&quot;Path to private key file&quot;)
def main(source, dest, verify, password, key_file):
    &quot;&quot;&quot;Transfer files or directories from SOURCE to DEST.&quot;&quot;&quot;
    src_path = Path(source)
    dst_path = Path(dest)
    is_sftp_source = source.startswith(&quot;sftp://&quot;)
    is_sftp_dest = dest.startswith(&quot;sftp://&quot;)
    client = None
    try:
        if is_sftp_source and is_sftp_dest:
            raise NotImplementedError(&quot;Direct SFTP to SFTP transfer not supported yet.&quot;)
        if is_sftp_source or is_sftp_dest:
            password = password or click.prompt(&quot;Enter SFTP password&quot;, hide_input=True)
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
        click.echo(&quot;Transfer completed successfully.&quot;)
    except Exception as e:
        click.echo(f&quot;Error: {e}&quot;, err=True)
        raise
    finally:
        if client:
            client.close()
if __name__ == &quot;__main__&quot;:
    main()