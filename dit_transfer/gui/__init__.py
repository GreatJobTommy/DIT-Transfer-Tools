import tkinter as tk
from tkinter import filedialog, ttk, messagebox, simpledialog
import threading
from pathlib import Path
from typing import Optional
import os

from dit_transfer.transfer import (
    parse_sftp_uri,
    sftp_connect,
    transfer_local_to_sftp,
    transfer_sftp_to_local,
    transfer_local,
)


def main():
    root = tk.Tk()
    root.title("DIT Transfer Tools - GUI")
    root.geometry("600x300")

    # Source
    tk.Label(root, text="Source (local file/dir):").grid(
    row=0, column=0, sticky="w", padx=5, pady=5
    )
    source_var = tk.StringVar(value=os.getcwd())
    source_entry = tk.Entry(root, textvariable=source_var, width=60)
    source_entry.grid(row=0, column=1, padx=5, pady=5)

    def browse_source():
    path = filedialog.askopenfilename(
        title="Select file"
    ) or filedialog.askdirectory(title="Select directory")
    if path:
        source_var.set(path)

    tk.Button(root, text="Browse", command=browse_source).grid(
    row=0, column=2, padx=5, pady=5
    )

    # Destination
    tk.Label(root, text="Destination (local or sftp://):").grid(
    row=1, column=0, sticky="w", padx=5, pady=5
    )
    dest_var = tk.StringVar()
    dest_entry = tk.Entry(root, textvariable=dest_var, width=60)
    dest_entry.grid(row=1, column=1, padx=5, pady=5)

    def browse_dest():
    path = filedialog.askdirectory(title="Select local directory")
    if path:
        dest_var.set(path)

    tk.Button(root, text="Browse Local", command=browse_dest).grid(
    row=1, column=2, sticky="w", padx=5, pady=5
    )

    # Options
    verify_var = tk.BooleanVar()
    tk.Checkbutton(
    root, text="Verify checksums after transfer", variable=verify_var
    ).grid(row=2, column=0, sticky="w", padx=5, pady=5)

    settings_frame = ttk.LabelFrame(root, text="Advanced LTO Settings")
    settings_frame.grid(row=2, column=0, columnspan=3, sticky="ew", padx=5, pady=5)
    buffer_label = tk.Label(settings_frame, text="Buffer Size (MB):")
    buffer_label.grid(row=0, column=0, sticky="w")
    buffer_var = tk.StringVar(value="64")
    buffer_entry = tk.Entry(settings_frame, textvariable=buffer_var, width=10)
    buffer_entry.grid(row=0, column=1, padx=5)
    seq_var = tk.BooleanVar()
    seq_check = tk.Checkbutton(settings_frame, text="Sequential (--inplace)", variable=seq_var)
    seq_check.grid(row=0, column=2, sticky="w", padx=10)

    rsync_var = tk.BooleanVar(value=True)  # LTO preset: rsync mode
    rsync_check = tk.Checkbutton(settings_frame, text="Rsync Fallback (metadata/xattrs for LTFS)", variable=rsync_var)
    rsync_check.grid(row=1, column=0, columnspan=2, sticky="w", padx=10, pady=5)

    # Progress
    progress = ttk.Progressbar(root, mode="indeterminate", length=500)
    progress.grid(row=3, column=0, columnspan=3, sticky="ew", padx=5, pady=5)

    status_label = tk.Label(root, text="Ready", fg="blue")
    status_label.grid(row=4, column=0, columnspan=3, pady=5)

    def do_transfer():
    source = source_var.get().strip()
    dest = dest_var.get().strip()
    if not source or not dest:
        messagebox.showerror("Error", "Please enter source and destination.")
        return
    verify = verify_var.get()
    password = None
    if dest.startswith("sftp://"):
        password = simpledialog.askstring(
            "SFTP Password", "Enter password:", show="*"
        )
        if password is None:
            return
    progress.start(10)
    status_label.config(text="Transferring...", fg="orange")
    root.update()
    buffer_size_mb = buffer_var.get()
    try:
    buffer_size = int(float(buffer_size_mb)) * 1024 * 1024
    except ValueError:
    messagebox.showerror("Error", "Invalid buffer size.")
    return
    thread = threading.Thread(
        target=transfer_worker,
        args=(
            source,
            dest,
            verify,
            buffer_size,
            seq_var.get(),
            rsync_var.get(),
            password,
            status_label,
            progress,
            root,
        ),
    )
    thread.daemon = True
    thread.start()

    def transfer_worker(
    source: str,
    dest: str,
    verify: bool,
    buffer_size: int,
    sequential_flags: bool,
    rsync_fallback: bool,
    password: Optional[str],
    status_label,
    progress,
    root,
    ):
    try:
        src_path = Path(source)
        dst_path = Path(dest)
        client = None
        if dest.startswith("sftp://"):
            user, host, port, remote_dest = parse_sftp_uri(dest)
            sftp, client = sftp_connect(user, host, port, password=password)
            transfer_local_to_sftp(src_path, remote_dest, sftp, verify=verify)
            if client:
                client.close()
            status_label.config(text="Upload completed successfully!", fg="green")
        elif source.startswith("sftp://"):
            # For prototype, password already gotten if needed, but assume dest local
            user, host, port, remote_src = parse_sftp_uri(source)
            sftp, client = sftp_connect(user, host, port, password=password)
            transfer_sftp_to_local(remote_src, dst_path, sftp, verify=verify)
            if client:
                client.close()
            status_label.config(text="Download completed successfully!", fg="green")
        else:
            # local to local
            transfer_local(
                src_path,
                dst_path,
                verify=verify,
                rsync_fallback=rsync_fallback,
                buffer_size=buffer_size,
                sequential_flags=sequential_flags,
            )
            status_label.config(
                text="Local transfer completed successfully!", fg="green"
            )
        root.after(0, lambda: progress.stop())
    except Exception as exc:
        root.after(0, lambda: progress.stop())
        root.after(
            0,
            lambda exc=exc: status_label.config(
                text=f"Error: {str(exc)}", fg="red"
            ),
        )
        root.after(
            0, lambda exc=exc: messagebox.showerror("Transfer Error", str(exc))
        )

    tk.Button(root, text="Start Transfer", command=do_transfer, bg="lightgreen").grid(
    row=5, column=1, pady=10
    )

    root.grid_columnconfigure(1, weight=1)
    root.mainloop()


if __name__ == "__main__":
    main()
