'''
Title: main/Views
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
# render used to render templates
from django.shortcuts import render, redirect
# HTTP Responses can be used
from django.http import HttpResponse
from api.models import Sensor, Patient, Value
from django.contrib.auth.forms import AuthenticationForm
from django.contrib.auth import login, logout, authenticate
from django.contrib.auth.models import Group
from django.contrib import messages
from .forms import NewUserForm
from django.views.generic import ListView, DetailView
from django.db.models import Q
from django.core import serializers
import json
from django.http import JsonResponse


##### Index Page #####


class HomeView(ListView):
    model = Patient
    template_name = 'main/home.html'


class SettingsView(ListView):
    model = Patient
    template_name = 'main/settings.html'


##### Patient Detail #####


class PatientDetailView(DetailView):
    model = Patient
    template_name = 'main/patient_detail.html'

    def get_context_data(self, **kwargs):
        context = super(PatientDetailView, self).get_context_data(**kwargs)
        context['all_objects'] = Patient.objects.all()

        return context


class SearchView(ListView):
    model = Patient
    template_name = 'main/search.html'

    def get_queryset(self):
        query = self.request.GET.get('search')
        object_list = Patient.objects.filter(
            Q(firstName__icontains=query) | Q(lastName__icontains=query)
        )
        return object_list

##### Register field #####


def register(request):
    if request.method == "POST":
        form = NewUserForm(request.POST)
        if form.is_valid():
            user = form.save()
            username = form.cleaned_data.get('username')
            group = Group.objects.get(name='Doctor')
            user.groups.add(group)
            messages.success(request, "New Account Created: %s" % username)
            login(request, user)
            return redirect("main:homepage")
        else:
            for msg in form.error_messages:
                messages.error(request, f"{msg}: {form.error_messages[msg]}")

    form = NewUserForm
    return render(request,
                  "main/register.html", context={"form": form})

##### Logout field #####


def logout_request(request):
    logout(request)
    messages.info(request, "Logged out successfully")
    return redirect("main:homepage")

##### Login field #####


def login_request(request):
    if request.method == "POST":
        form = AuthenticationForm(request, data=request.POST)
        if form.is_valid():
            username = form.cleaned_data["username"]
            password = form.cleaned_data["password"]
            user = authenticate(username=username, password=password)
            if user is not None:
                login(request, user)
                messages.info(request, f"Your now logged in as {username}")
                return redirect("main:homepage")
            else:
                messages.error(request, f"Invalid username or password")

    form = AuthenticationForm()
    return render(request, "main/login.html", {"form": form})
