__license__ = 'MIT'
__copyright__ = '2024'

from calibre.customize import EditBookToolPlugin


class GrayscalePlugin(EditBookToolPlugin):

    name = 'Grayscale plugin'
    version = (1, 0, 0)
    author = 'SEbook'
    supported_platforms = ['windows', 'osx', 'linux']
    description = 'Grayscales images'
    minimum_calibre_version = (7, 0, 0)
