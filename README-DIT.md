DIT Results from OpenClaw Team

## Rclone Setup

To use cloud storage destinations (S3, Google Drive, etc.), install rclone and configure remotes.

### Installation

```bash
# On Ubuntu/Debian
sudo apt install rclone

# Or download from https://rclone.org/downloads/
```

### Configuration

Run `rclone config` to set up remotes.

Example for Google Drive:
1. `rclone config`
2. `n` for new remote
3. Name: `gdrive`
4. Type: `drive`
5. Follow prompts for OAuth

Example for S3:
1. `rclone config`
2. `n` for new remote
3. Name: `s3`
4. Type: `s3`
5. Enter access key, secret, region, etc.

### Usage in DIT

In Add Task dialog, set destination to `remote_name:/path`, e.g., `gdrive:/backup` or `s3:/mybucket`.

The app uses `rclone copy --checksum` for transfers with retry on failure.

Config path can be set in settings (default: ~/.config/rclone/rclone.conf).
