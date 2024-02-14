from calibre.gui2 import error_dialog
import calibre_plugins.ebook_image
from PyQt5.Qt import QDialog, QVBoxLayout, QTextEdit, QPushButton
from bs4 import BeautifulSoup
import zipfile
def GrayScale(db, book_title,QDialog):
    #this is probably suboptimal, but it works for now
    all_books = db.all_book_ids(list)    
    ID = 0  
    for book_id in all_books:
        book = db.get_metadata(book_id)
        title = book.get('title')
        if title == book_title:
            ID = book_id
            break
    #documentation recommends using copy_format_to instead but no idea where that is defined
    content = db.format_abspath(ID, 'EPUB')
    images = []
    #using zipfile to load the epub
    with zipfile.ZipFile(content) as epub:
        for item in epub.infolist():
            print(item.filename)
            if item.filename.endswith('.jpg'):
                with epub.open(item.filename) as page:
                    print('enters')
                    images.append(item.filename)
    print(len(images))