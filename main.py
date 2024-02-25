
from calibre_plugins.ebook_image.process import GrayScale_Epub
from calibre.gui2.actions import InterfaceAction
from calibre.customize import InterfaceActionBase
from PyQt5.Qt import QToolButton, QPixmap, QMenu,QIcon
import calibre.customize
import calibre_plugins.ebook_image
from calibre.library import db
from PyQt5.QtWidgets import (QDialog, QPushButton, QVBoxLayout, QLabel, QMessageBox, 
QListWidget, QDialogButtonBox, QComboBox,QProgressBar, QApplication)
from time import sleep
class bar(QDialog):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout()
        self.progress = QProgressBar()
        self.progress.show()
        self.setGeometry(350, 500, 250, 50)
        self.done_button = QPushButton('Done', self)
        #disables the done button until the process is complete
        self.done_button.setEnabled(False)
        self.done_button.clicked.connect(self.done)
        layout.addWidget(self.progress)
        layout.addWidget(self.done_button)
        self.setLayout(layout)
        self.setWindowTitle('Working')
    def done(self):
        self.hide()

class box(QDialog):
    def about(self):
            text = get_resources('about.txt')
            QMessageBox.about(self, 'About Charcoal',
                text.decode('utf-8'))
    def populate_book_list(self):
        # Fetch book data from Calibre library
        db = self.gui.current_db.new_api
        all_books = db.all_book_ids(list)
        
        
        #Display book titles in the list widget
        for book_id in all_books:
            book = db.get_metadata(book_id)
            title = book.get('title')
            self.book_list_widget.addItem(title)

    def accept(self):
        #handles the case where no book is selected
        if self.book_list_widget.selectedItems() == []:
            msgBox = QMessageBox()
            msgBox.setText("Please select an Ebook to grayscale/compress.")
            msgBox.setWindowTitle("ERROR: NO SELECTION")
            msgBox.setStandardButtons(QMessageBox.Ok)
            msgBox.exec()
            return
        db = self.gui.current_db.new_api
        #Image format specified by the user to be converted to
        image_type = '.'+self.image_type_button.currentText().lower()

        #get selected image quality, reduce 100 to 95
        size = int(self.size_button.currentText().replace('%', ''))
        if size == 100:
            size = 95
        #now handles multiple book selection
        self.bar.progress.setValue(0)
        self.bar.show()
        #fixes progress bar delay
        QApplication.processEvents()
        comp = 0
        selected_books = self.book_list_widget.selectedItems()
        for books in selected_books:
<<<<<<< Updated upstream
            #calls grayscale function from process.py
            comp = GrayScale_Epub(db, books.text(), size, len(selected_books), comp, self)
=======
            ID = 0  
            for book_id in all_books:
                meta = db.get_metadata(book_id)
                title = meta.get('title')
                if title == books.text():
                    ID = book_id
                    format = db.formats(ID)
                    print(format)
                    print(type(format))
                    break
            #Switch out the Grayscale for the appropriate function
            match format:
                case ('EPUB',):
                    comp = GrayScale_Epub(db, ID, size, len(selected_books), comp, self, image_type)
                case ('AZW3',):
                    comp = GrayScale_Epub(db, ID, size, len(selected_books), comp, self, image_type)
                case ('PDF',):
                    comp = GrayScale_Epub(db, ID, size, len(selected_books), comp, self, image_type)

>>>>>>> Stashed changes
        #accounts for rounding issues
        if self.bar.progress.value() != 100:
            self.bar.progress.setValue(100)
        #enables the done button once the process is complete
        self.bar.done_button.setEnabled(True)
        self.book_list_widget.clearSelection()
    def __init__(self, gui, icon, do_user_config):
        self.bar = bar()
        QDialog.__init__(self, gui)
        self.gui = gui
        self.setWindowTitle('Select Book')
        
        self.layout = QVBoxLayout(self)

        self.about_button = QPushButton('About', self)
        self.about_button.clicked.connect(self.about)
        self.layout.addWidget(self.about_button)
        
        button_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        button_box.accepted.connect(self.accept)
        button_box.rejected.connect(self.reject)
        self.layout.addWidget(button_box)

        self.book_list_widget = QListWidget()
        #added support for multiple book selection
        self.book_list_widget.setSelectionMode(QListWidget.MultiSelection)

        self.layout.addWidget(self.book_list_widget)

        self.size_label = QLabel("Image Quality:")
        self.layout.addWidget(self.size_label)
        self.size_button = QComboBox()
        self.size_button.addItem("100%")
        self.size_button.addItem("75%")
        self.size_button.addItem("50%")
        self.size_button.addItem("25%")
        self.layout.addWidget(self.size_button)

        self.image_type_label = QLabel("Image Type:")
        self.layout.addWidget(self.image_type_label)
        self.image_type_button = QComboBox()
        self.image_type_button.addItem("PNG")
        self.image_type_button.addItem("JPG")
        self.layout.addWidget(self.image_type_button)

        self.populate_book_list()

        self.resize(self.sizeHint())
class GrayScaleAction(InterfaceAction):
    name = 'gray-scale'
    allowed_in_toolbar = True 
    allowed_in_menu = True
    action_spec = ('Charcoal', 'charcoal.png', 'Grayscales Ebooks', None)
    description = "grayscale"
    #initializes the plugin
    def genesis(self, for_toolbar=True):
        self.is_library_selected = True
        #sets icon for button
        icon = get_icons('icons/charcoal.png')
        if (icon.isNull()):
            print("\n BROKEN\n")
        self.qaction.setIcon(icon)
        self.qaction.triggered.connect(self.grayscale_dialog)
    #always called after initialization
    def initialization_complete(self):
        print("init complete")
    #where the plugin funcionality is actually defined 
    def grayscale_dialog(self):
        base_plugin_object = self.interface_action_base_plugin
        do_user_config = base_plugin_object.do_user_config

        i = box(self.gui, self.qaction.icon(), do_user_config)
        i.show()


   