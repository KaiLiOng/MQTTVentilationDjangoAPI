'''
Title: URLS
Version: v1
Date: 14.12.2020

Short Description:
URL Routing for the webapp is set here.
Several URL path are connected to functions from other scripts.

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''

##############################################################################
############################### Imports ######################################
from django.urls import path
from . import views

##### Patterns #####

urlpatterns = [
    path('', views.apiOverview, name='apiOverview'),
    path('getConfig', views.configGet, name='getConfig'),
    path('sensor-list', views.sensorList, name='sensor-list'),
    path('sensor-detail/<str:pk>/', views.sensorDetail, name='sensor-detail'),
    path('sensor-create', views.sensorCreate, name='sensor-create'),
    path('sensor-update/<str:pk>/', views.sensorUpdate, name='sensor-update'),
    path('sensor-delete/<str:pk>/', views.sensorDelete, name='sensor-delete'),
    path('patient-list', views.patientList, name='sensor-list'),
    path('patient-detail/<str:pk>/', views.patientDetail, name='sensor-detail'),
    path('patient-create', views.patientCreate, name='sensor-create'),
    path('patient-update/<str:pk>/', views.patientUpdate, name='sensor-update'),
    path('patient-delete/<str:pk>/', views.patientDelete, name='sensor-delete'),
    path('value-list', views.valueList, name='sensor-list'),
    path('value-detail/<str:pk>/', views.valueDetail, name='sensor-detail'),
    path('value-create', views.valueCreate, name='value-create'),
    path('value-update/<str:pk>/', views.valueUpdate, name='sensor-update'),
    path('value-delete/<str:pk>/', views.valueDelete, name='sensor-delete'),
    path('detailValueInfo/<str:patientID>/<str:sensorName>',
         views.valueDetialInformation, name='detailValueInfo'),
]
