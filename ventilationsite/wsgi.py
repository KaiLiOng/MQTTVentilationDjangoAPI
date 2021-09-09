'''
Title: WSGI
Version: v1
Date: 14.12.2020

Short Description:
WSGI config for ventilationsite project.
It exposes the WSGI callable as a module-level variable named ``application``.
For more information on this file, see: https://docs.djangoproject.com/en/3.1/howto/deployment/wsgi/

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''


##############################################################################
############################### Imports ######################################
import os
from django.core.wsgi import get_wsgi_application



os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'ventilationsite.settings')
application = get_wsgi_application()
