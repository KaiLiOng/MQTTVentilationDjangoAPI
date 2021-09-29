'''
Title: main/Apps
Version: v1
Date: 14.12.2020

Short Description:
Webapp forms like register and log in are configurated here.

Output Data:

Devices to connect:

Changelog:
14.12.2020 First Test Implementation

'''

##############################################################################
############################### Imports ######################################
from django.contrib.auth.models import User
from django.contrib.auth.forms import UserCreationForm
from django import forms


##### Register and Log In Form #####
class NewUserForm(UserCreationForm):
    email = forms.EmailField(required=True)

    class Meta:
        model = User
        fields = ('username', 'email', 'last_name',
                  'first_name', 'password1', 'password2')

    def save(self, commit=True):
        user = super(NewUserForm, self).save(commit=False)
        user.email = self.cleaned_data['email']
        user.is_staff = True
        if commit:
            user.save()
        return user
