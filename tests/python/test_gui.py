import pytest
from unittest.mock import patch, MagicMock

from dit_transfer.gui import main

import os

@pytest.mark.skipif("CI" in os.environ, reason="Headless, no Tk root")
def test_gui_main():
    """Test that GUI main function runs without crashing."""
    with (
        patch("dit_transfer.gui.tk.Tk") as mock_Tk,
        patch("dit_transfer.gui.tk.StringVar") as mock_StringVar,
        patch("dit_transfer.gui.tk.BooleanVar") as mock_BooleanVar,
        patch("dit_transfer.gui.tk.Label") as mock_Label,
        patch("dit_transfer.gui.tk.Entry") as mock_Entry,
        patch("dit_transfer.gui.tk.Button") as mock_Button,
        patch("dit_transfer.gui.tk.Checkbutton") as mock_Checkbutton,
        patch("dit_transfer.gui.ttk.Progressbar") as mock_Progressbar,
        patch("dit_transfer.gui.filedialog.askopenfilename"),
        patch("dit_transfer.gui.filedialog.askdirectory"),
        patch("dit_transfer.gui.simpledialog.askstring", return_value=None),
        patch("dit_transfer.gui.threading.Thread") as mock_thread,
    ):
        # All widget mocks return MagicMock instances
        mock_StringVar.return_value = MagicMock()
        mock_BooleanVar.return_value = MagicMock()
        mock_Label.return_value = MagicMock()
        mock_Entry.return_value = MagicMock()
        mock_Button.return_value = MagicMock()
        mock_Checkbutton.return_value = MagicMock()
        mock_Progressbar.return_value = MagicMock()

        mock_root = MagicMock()
        mock_root.mainloop.return_value = None
        mock_root.title.return_value = None
        mock_root.geometry.return_value = None
        mock_root.grid_columnconfigure.return_value = None
        mock_root.update.return_value = None
        mock_root.after.return_value = None
        mock_Tk.return_value = mock_root
        mock_thread.return_value = MagicMock()
        main()
        mock_root.mainloop.assert_called_once()