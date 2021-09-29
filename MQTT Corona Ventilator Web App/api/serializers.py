'''
Title: Serializers
Version: v1
Date: 14.12.2020

Short Description:
Serializers are used to serialize data into json format.
Modelserializers include create and updating methods

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''


##############################################################################
############################### Imports ######################################
from rest_framework import serializers
from .models import Patient, Sensor, Value

##### Sensor Serializers #####


class SensorSerializer(serializers.ModelSerializer):
    class Meta:
        model = Sensor
        fields = '__all__'


##### Patient Serializers #####
class PatientSerializer(serializers.ModelSerializer):
    class Meta:
        model = Patient
        fields = '__all__'


##### Value Serializers #####
class ValueSerializer(serializers.ModelSerializer):
    sensors = SensorSerializer(read_only=True, many=True)

    class Meta:
        model = Value
        fields = '__all__'
