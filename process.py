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
def GrayScale_Epub(db, book_title, size, numbooks, comp, QDialog):
    #this can probably be done more efficiently by passing the ID directly from main
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
    #using zipfile to load the epub
    #as far as I can tell you need to extract the files to a temporary directory to modify them
    #this seems clunky so if a workaround could be found that would be nice
    temp = tempfile.TemporaryDirectory()
    epub = zipfile.ZipFile(content)
    epub_title = os.path.abspath(content)
    epub.extractall(temp.name)
    for item in epub.infolist():
        #incrementing the progress bar
        comp += (100//numbooks//len(epub.infolist()))+1
        QDialog.bar.progress.setValue(comp)
        if item.filename.endswith('.jpg') or item.filename.endswith('.png'):
            with epub.open(item.filename) as page:
                path = os.path.join(temp.name, item.filename)
                #using pillow to convert the image to greyscale
                image = Image.open(page)
                image = image.convert('L')
                #now saves images to the specified quality
                image.save(path, quality=size)
    #saving the modified files to a new epub that will replace the old epub
    with zipfile.ZipFile(epub_title, 'w') as new_epub:
        for root, sub, files in os.walk(temp.name):
            for i in range(len(sub)):
                new_epub.mkdir(sub[i])
                Meta = sub[0]
                OEBPS = sub[1]
            for file in files:
                if os.path.basename(root) == 'OEBPS' or os.path.basename(root) == 'META-INF':
                    path = os.path.join(root, file)
                    file = os.path.join(os.path.basename(root), os.path.basename(file))
                else:
                    path = os.path.join(root, file)
                new_epub.write(str(path), file)
    temp.cleanup()
    return comp


