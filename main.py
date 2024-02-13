

from calibre.gui2.actions import InterfaceAction
from calibre.customize import InterfaceActionBase
from PyQt5.Qt import QToolButton, QPixmap, QMenu,QIcon
import calibre.customize
import calibre_plugins.ebook_image

class GrayScaleAction(InterfaceAction):
    name = 'gray-scale'
    allowed_in_toolbar = True 
    allowed_in_menu = True
    action_spec = ('Charcoal', 'charcoal.png', 'Grayscales Ebooks', None)
    description = "grayscale"
    #initializes the plugin
    def genesis(self, for_toolbar=True):
        self.is_library_selected = True
        self.button = QToolButton()
        self.button.setToolTip('grayscale books')
        self.button.clicked.connect(self.grayscale)
        #sets icon for button
        icon = get_icons('icons/charcoal.png')
        if (icon.isNull()):
            print("\n BROKEN\n")
        self.qaction.setIcon(icon)
    #always called after initialization
    def initialization_complete(self):
        print("init complete")
    #where the plugin funcionality is actually defined 
    def grayscale(self):
        print("works")