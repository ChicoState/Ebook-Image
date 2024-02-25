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
def GrayScale_Epub(db, ID, size, numbooks, comp, QDialog, image_type):
    #this can probably be done more efficiently by passing the ID directly from main

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
                path = os.path.join(temp.name, item.filename)+image_type
                print(type(path))
                #using pillow to convert the image to greyscale
                image = Image.open(page)
                image = image.convert('L')
                #now saves images to the specified quality
                image.save(path, quality=size)
    #saving the modified files to a new epub that will replace the old epub
    #now accounts for singularly nested directories
    dir = [""]
    with zipfile.ZipFile(epub_title, 'w') as new_epub:
        for root, sub, files in os.walk(temp.name):
            for i in range(len(sub)):
                dir.append(sub[i])
            for file in files:
                found = False
                base = root
                path = file
                for i in range (len(dir)):
                    if os.path.basename(base) == dir[i]:
                        found = True
                    while found:
                        path = os.path.join(os.path.basename(base), path)
                        old = base
                        base = os.path.basename(os.path.dirname(base))
                        found = False
                        for j in range (len(dir)):
                            if base == dir[j] and old != base:
                                print(dir[i])
                                found = True
                file = os.path.join(root, os.path.basename(file))
                new_epub.write(str(file), path)
                            
    
    temp.cleanup()
    return comp


