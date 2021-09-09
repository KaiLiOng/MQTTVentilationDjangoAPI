'''
Title: Manage
Version: v1
Date: 14.12.2020

Short Description:
Django's command-line utility for administrative tasks.
#!/usr/bin/env python

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''


##############################################################################
############################### Imports ######################################
import os
import sys


def main():                                                                                         # Run administrative tasks
    os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'ventilationsite.settings')
    try:
        from django.core.management import execute_from_command_line
    except ImportError as exc:
        raise ImportError(
            "Couldn't import Django. Are you sure it's installed and "
            "available on your PYTHONPATH environment variable? Did you "
            "forget to activate a virtual environment?"
        ) from exc
    execute_from_command_line(sys.argv)


if __name__ == '__main__':
    main()
