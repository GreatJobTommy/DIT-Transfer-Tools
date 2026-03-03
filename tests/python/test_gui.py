import pytest
from unittest.mock import patch, MagicMock

from dit_transfer.gui import main


@pytest.fixture
def mock_root():
    with patch("tkinter.Tk") as mock_tk:
        mock_root = MagicMock()
        mock_root.mainloop.return_value = None
        mock_tk.return_value = mock_root
        return mock_root


def test_gui_main(mock_root):
    """Test that GUI main function runs without crashing."""
    with patch("tkinter.filedialog.askopenfilename"), \
         patch("tkinter.filedialog.askdirectory"), \
         patch("tkinter.simpledialog.askstring", return_value=None), \
         patch("threading.Thread") as mock_thread:
        mock_thread.return_value = MagicMock()
        main()
    assert mock_root.mainloop.called