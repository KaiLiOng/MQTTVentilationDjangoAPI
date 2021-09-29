'''
Title: Admin
Version: v1
Date: 14.12.2020

Short Description:
All used models for the database have to be registered here.

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''

##############################################################################
############################### Imports ######################################

from django.contrib import admin
from .models import Patient, Sensor, Value, ChartType

# Register your models here.
admin.site.register(Patient)
admin.site.register(Sensor)
admin.site.register(Value)
admin.site.register(ChartType)
