#!/usr/bin/env python
# vim:fileencoding=utf-8


__license__ = ' MIT'
__copyright__ = '2024, SEgroup'

from calibre.customize import EditBookToolPlugin


class GrayscalePlugin(EditBookToolPlugin):

    name = 'Image Grayscaler'
    version = (1, 0, 0)
    author = 'SEgroup'
    supported_platforms = ['windows', 'osx', 'linux']
    description = 'First version of SEgroup plugin'
    minimum_calibre_version = (7, 0, 0)