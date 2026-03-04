import pytest
from unittest.mock import patch, MagicMock

from dit_transfer.gui import main


import os
import pytest

@pytest.mark.skipif("CI" in os.environ, reason="Headless, no Tk root")
def test_gui_main():
    """Test that GUI main function runs without crashing."""
    with (
        patch("dit_transfer.gui.tkinter.Tk") as mock_Tk,
        patch("dit_transfer.gui.tkinter.filedialog.askopenfilename"),
        patch("dit_transfer.gui.tkinter.filedialog.askdirectory"),
        patch("dit_transfer.gui.tkinter.simpledialog.askstring", return_value=None),
        patch("dit_transfer.gui.threading.Thread") as mock_thread,
    ):
        mock_root = MagicMock()
        mock_root.mainloop.return_value = None
        mock_Tk.return_value = mock_root
        mock_thread.return_value = MagicMock()
        main()
        mock_root.mainloop.assert_called_once()