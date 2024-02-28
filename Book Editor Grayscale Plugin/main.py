__license__ = 'MIT'
__copyright__ = '2024'

import io
from PIL import Image
from qt.core import QAction, QInputDialog

# The base class that all tools must inherit from
from calibre.gui2.tweak_book.plugin import Tool

class Grayscale(Tool):

    name = 'grayscale'
    allowed_in_toolbar = True
    allowed_in_menu = True

    def create_action(self, for_toolbar=True):
        ac = QAction(get_icons('icons/charcoal.png'), 'Convert to Grayscale', self.gui)
        if not for_toolbar:
            self.register_shortcut(ac, 'convert-to-grayscale', default_keys=('Ctrl+Shift+Alt+G',))
        ac.triggered.connect(self.ask_user)
        return ac

    def ask_user(self):
        container = self.current_container
        self.convert_images_to_grayscale(container)
        self.boss.apply_container_update_to_gui()

    def convert_images_to_grayscale(self, container):
        for name, media_type in container.mime_map.items():
            if media_type.startswith('image/'):
                with container.open(name, 'rb') as f:
                    image_data = f.read()
                with Image.open(io.BytesIO(image_data)) as img:
                    grayscale_img = img.convert('L')
                with container.open(name, 'wb') as f:
                    grayscale_img.save(f, format=img.format)

