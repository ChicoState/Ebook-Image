#!/usr/bin/env python
# vim:fileencoding=utf-8


__license__ = ' MIT'
__copyright__ = '2024, SEgroup'

from calibre.customize import InterfaceActionBase
#must be imported in order to access other files within directory
from calibre_plugins.ebook_image import main

class GrayScalePlugin(InterfaceActionBase):

    name = 'Image Grayscaler'
    version = (1, 0, 1)
    author = 'SEgroup'
    supported_platforms = ['windows', 'osx', 'linux']
    description = 'First version of SEgroup plugin'
    minimum_calibre_version = (7, 0, 0)

    #references class defined in main.py
    actual_plugin = 'calibre_plugins.ebook_image.main:GrayScaleAction'