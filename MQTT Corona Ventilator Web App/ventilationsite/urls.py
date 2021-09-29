'''
Title: ventilationsite/URLS
Version: v1
Date: 14.12.2020

Short Description:
URL Routing for the webapp is set here.
Several URL path are connected to functions from other scripts.
More information: https://docs.djangoproject.com/en/3.1/topics/http/urls/

Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''


##############################################################################
############################### Imports ######################################

from django.contrib import admin
from django.urls import path, include                                                           # needed to use path() and include()


urlpatterns = [                                                                                 # The `urlpatterns` list routes URLs to views. For more information please see: https://docs.djangoproject.com/en/3.1/topics/http/urls/
    path('', include('main.urls')),                                                             # when just homepage is entered -> main.urls is used
    path('admin/', admin.site.urls),
]
