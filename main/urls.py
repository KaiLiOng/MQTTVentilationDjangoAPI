'''
Title: main/URLS
Version: v1
Date: 14.12.2020

Short Description:
URL Routing for the webapp is set here.
Several URL path are connected to functions from other scripts.

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

from django.urls import path, include
# views included to  use their functions in urlpatterns
from . import views
from .views import HomeView, PatientDetailView, SearchView, SettingsView

app_name = "main"

urlpatterns = [                                                                 # The `urlpatterns` list routes URLs to views. For more information please see: https://docs.djangoproject.com/en/3.1/topics/http/urls/


    # "" empty path points to HomeView() Function
    path('', HomeView.as_view(), name="homepage"),
    path("patient/<int:pk>", PatientDetailView.as_view(), name='patient-detail'),
    # PK == Primary KEY
    # "register" path points to register function from views
    path("register/", views.register, name="register"),
    # "logout" path points to register function from views
    path("logout/", views.logout_request, name="logout"),
    # "login" path points to register function from views
    path("login/", views.login_request, name="login"),
    path("settings/", SettingsView.as_view(), name="settings"),
    # when just patients is entered -> api.urls is included
    path("api/", include('api.urls')),
    path('search/', SearchView.as_view(), name='search'),
]
