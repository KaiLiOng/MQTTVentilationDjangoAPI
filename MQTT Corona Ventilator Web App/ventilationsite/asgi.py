'''
Title: ASGI
Version: v1
Date: 14.12.2020

Short Description:
ASGI config for ventilationsite project.
It exposes the ASGI callable as a module-level variable named ``application``.
For more information on this file, see: https://docs.djangoproject.com/en/3.1/howto/deployment/asgi/

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''

##############################################################################
############################### Imports ######################################
import os
from django.core.asgi import get_asgi_application



os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'ventilationsite.settings')
application = get_asgi_application()
