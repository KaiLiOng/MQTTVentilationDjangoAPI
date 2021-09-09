'''
Title: Models
Version: v1
Date: 14.12.2020

Short Description:
Models are used from the webside to interact with stored data in database.

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''

##############################################################################
############################### Imports ######################################

# every model created here inherits from django "base" model
from django.db import models
from datetime import datetime
from django.contrib.auth.models import Group
from django.conf import settings
from django.utils.text import slugify


# Creating new table in database with the columns --> default set primary key
# by adding or chaging table ------> python manage.py makemigration !!!!
# ./manage.py makemigrations <myapp>
# and then python manage.py migrate

# if you mnake changes in SQL database
# delete migrations folder
# delete db.SQL Lite 3
# delete files in pycache -> main -> folder __pycache__ delete all files
# main -> folder migration -> delete migration -> than new folder "migrations" and new file "__init__.py"
# python manage.py migrate
# python manage.py makemigrations
# python manage.py migrate
# python manage.py createsuperuser
# python manage.py migrate --run-syncdb
# run again

##### Sensor #####
# Sensor Model inherits from django "base" model


class ChartType(models.Model):
    chart_type = models.CharField(max_length=200, choices=[
                                  ("bar", 'bar'), ('pie', 'pie'), ("line", 'line'), ('text', 'text'), ], default='bar', primary_key=True)

    def __str__(self):
        return self.chart_type


class Sensor(models.Model):
    sensor_name = models.CharField(max_length=200, primary_key=True)
    chart_type = models.ManyToManyField(ChartType)
    xAxisTitle = models.TextField(blank=True, max_length=200)
    compareValue = models.IntegerField()

    def __str__(self):
        return self.sensor_name

# Patient Model inherits from django "base" model


class Patient(models.Model):
    firstName = models.CharField(max_length=200)
    lastName = models.CharField(max_length=200)
    age = models.IntegerField()
    doctor = models.ForeignKey(
        settings.AUTH_USER_MODEL, on_delete=models.CASCADE)
    # Information in Admin page
    sensor = models.ManyToManyField(Sensor)

    class Meta:
        verbose_name_plural = "Patients"

    def __str__(self):
        return self.firstName + " " + self.lastName


# Value Model inherits from django "base" model
class Value(models.Model):
    value_date = models.DateTimeField(auto_now_add=True)
    value = models.FloatField()
    sensor = models.ForeignKey(Sensor, on_delete=models.CASCADE)
    patient = models.ForeignKey(Patient, on_delete=models.CASCADE)
