from calibre.gui2 import error_dialog
import calibre_plugins.ebook_image
from PyQt5.Qt import QDialog, QVBoxLayout, QTextEdit, QPushButton
from bs4 import BeautifulSoup
import zipfile
from PIL import Image
from pathlib import Path
import os
import shutil
import tempfile
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
    #as far as I can tell you need to extract the files to a temporary directory to modify them
    #this seems clunky so if a workaround could be found that would be nice
    temp = tempfile.TemporaryDirectory()
    epub = zipfile.ZipFile(content)
    epub.extractall(temp.name)
    for item in epub.infolist():
        if item.filename.endswith('.jpg'):
            with epub.open(item.filename) as page:
                print('enters')
                images.append(item.filename)
                path = os.path.join(temp.name, item.filename)
                #using pillow to convert the image to greyscale
                image = Image.open(page)
                grey = image.convert('L')
                image.save(path)
    with zipfile.ZipFile("new_epub", 'w') as new_epub:
        for root, sub, files in os.walk(temp.name):
            for i in range(len(sub)):
                new_epub.mkdir(sub[i])
                print (sub[i])
                Meta = sub[0]
                OEBPS = sub[1]
            for file in files:
                if len(files) > 1:
                    print('there')
                    path = os.path.join(root, file)
                    print(sub)
                    file = os.path.join(OEBPS, os.path.basename(file))
                else:
                    print('here')
                    path = os.path.join(root, file)
                print(path)
                print(file)
                new_epub.write(str(path), file)

    temp.cleanup()