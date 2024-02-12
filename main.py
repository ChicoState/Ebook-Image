

from calibre.gui2.actions import InterfaceAction
from calibre.customize import InterfaceActionBase
from PyQt5.Qt import QToolButton, QPixmap, QMenu,QIcon


class GrayScaleAction(InterfaceAction):
    name = 'grey-scale'
    print("\nWHY\n")
    allowed_in_toolbar = True

    allowed_in_menu = True
    description = "greyscale"
    def genesis(self, for_toolbar=True):
        print("\ngets\n")
        self.is_library_selected = True
        self.button = QToolButton()
        pixmap = QPixmap('icons/charcoal.png')
        if pixmap.isNull():
            print("\nBROKEN\n")
        self.button.setIcon(QIcon(pixmap))
        self.button.setToolTip('greyscale books')
        self.button.clicked.connect(self.greyscale)
    def initialization_complete(self):
        print("init complete")
    def greyscale(self):
        print("works")