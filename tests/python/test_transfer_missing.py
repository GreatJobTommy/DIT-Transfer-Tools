import os
import pytest
import random
from unittest.mock import Mock, patch
from pathlib import Path
from dit_transfer.transfer import (
    sftp_connect,
    spot_check_verify,
    transfer_local,
    hash_chunk,
)