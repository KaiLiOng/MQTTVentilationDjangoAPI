'''
Title: Views
Version: v1
Date: 14.12.2020

Short Description:
In this script "view functions" are defined which take web request and return
a web response which can be html content or a redirect.

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''


##############################################################################
############################### Imports ######################################
from django.shortcuts import render
from django.http import JsonResponse

from rest_framework.decorators import api_view
from rest_framework.response import Response
from .serializers import SensorSerializer, ValueSerializer, PatientSerializer
from .models import Sensor, Patient, Value
from django.http import JsonResponse
import json


@api_view(['GET'])
def apiOverview(request):
    api_urls = {
        'List': '/sensor-list',
        'Detail View': 'sensor-detail/<str:pk>/',
        'Create': 'sensor-create'
    }

    return Response(api_urls)


##############################################################################
#  Sensor
##############################################################################

# Overview all Sensors
@api_view(['GET'])
def sensorList(request):
    sensors = Sensor.objects.all()
    serializer = SensorSerializer(sensors, many=True)
    return Response(serializer.data)

# Detail-View of Sensor


@api_view(['GET'])
def sensorDetail(request, pk):
    sensors = Sensor.objects.get(id=pk)
    serializer = SensorSerializer(sensors, many=False)
    return Response(serializer.data)

# Creating new Sensor via API


@api_view(['POST'])
def sensorCreate(request):
    serializer = SensorSerializer(data=request.data)

    if serializer.is_valid():
        serializer.save()
    return Response(serializer.data)

# Update Sensor if changes are required like new name or other refresh rate


@api_view(['POST'])
def sensorUpdate(request, pk):
    sensors = Sensor.objects.get(id=pk)
    serializer = SensorSerializer(instance=sensors, data=request.data)

    if serializer.is_valid():
        serializer.save()
    return Response(serializer.data)

# For deleting sensors


@api_view(['DELETE'])
def sensorDelete(request, pk):
    sensors = Sensor.objects.get(id=pk)
    sensors.delete()
    return Response("Sensor was deleted")


##############################################################################
#  Patient
##############################################################################

# Overview all Patients
@api_view(['GET'])
def patientList(request):
    patients = Patient.objects.all()
    serializer = PatientSerializer(patients, many=True)
    return Response(serializer.data)

# Detail-View of Patients


@api_view(['GET'])
def patientDetail(request, pk):
    patients = Patient.objects.get(id=pk)
    serializer = PatientSerializer(patients, many=False)
    return Response(serializer.data)

# Detail-View of Patients


@api_view(['GET'])
def patientDetail2(request, pk, sensorName):
    patients = Patient.objects.get(id=pk, sensors=sensorName)
    serializer = PatientSerializer(patients, many=False)
    return Response(serializer.data)

# Creating new Patient via API


@api_view(['POST'])
def patientCreate(request):
    serializer = PatientSerializer(data=request.data)

    if serializer.is_valid():
        serializer.save()
    return Response(serializer.data)

# Update Patient if changes are required like new name or other refresh rate


@api_view(['POST'])
def patientUpdate(request, pk):
    patients = Patient.objects.get(id=pk)
    serializer = PatientSerializer(instance=sensors, data=request.data)

    if serializer.is_valid():
        serializer.save()
    return Response(serializer.data)

# For deleting Patients


@api_view(['DELETE'])
def patientDelete(request, pk):
    patients = Patient.objects.get(id=pk)
    patients.delete()
    return Response("Sensor was deleted")


##############################################################################
#  Values
##############################################################################

# Overview all Values
@api_view(['GET'])
def valueList(request):
    values = Value.objects.all()
    serializer = ValueSerializer(values, many=True)
    return Response(serializer.data)

# Detail-View of Values


@api_view(['GET'])
def valueDetail(request, pk):
    values = Value.objects.get(id=pk)
    serializer = ValueSerializer(values, many=False)
    return Response(serializer.data)

# Creating new Value via API


@api_view(['POST'])
def valueCreate(request):
    serializer = ValueSerializer(data=request.data, many=True)
    if serializer.is_valid():
        serializer.save()
    return Response(serializer.data)

# Update Value if changes are required like new name or other refresh rate


@api_view(['POST'])
def valueUpdate(request, pk):
    values = Value.objects.get(id=pk)
    serializer = ValueSerializer(data=request.data)

    if serializer.is_valid():
        serializer.save()

    return Response(serializer.data)

# For deleting Values


@api_view(['DELETE'])
def valueDelete(request, pk):
    values = Value.objects.get(id=pk)
    values.delete()
    return Response("Sensor was deleted")


@api_view(['GET'])
def configGet(request):
    return JsonResponse({"foo": "bar"})

# Function get called from Graph function via ajax request


@api_view(['GET'])
def valueDetialInformation(request, patientID, sensorName):
    values = Value.objects.filter(patient=patientID, sensor=sensorName).order_by(
        'value_date').reverse()[:1]
    data = json.dumps([{'value': o.value, } for o in values])
    return JsonResponse(data, safe=False)
