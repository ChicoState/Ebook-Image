

# The base class that all tools must inherit from
from calibre.gui2.actions import InterfaceAction




class GreyScaleACtion(InterfaceAction):
    name = 'grey-scale'

    allowed_in_toolbar = True

    allowed_in_menu = True

    def create_action(self):
