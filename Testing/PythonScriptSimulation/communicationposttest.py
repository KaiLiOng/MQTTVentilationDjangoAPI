#!/usr/bin/env python3

import os
import sys
import requests
import math
import json
import time

host = "192.168.188.157"
port = "8887"

def generateNumber(i):
    
    sinusnumber = round(math.sin(i*0.0174533),4)
    #print(str(i) + " :" + str(sinusnumber))
    json_string = json.dumps([{"value": sinusnumber, "sensor": "InFlow", "patient": 2}])
    my_json_string = json_string
    #json_string = None
    header = {'Content-type': 'application/json'}
    #r = requests.post("https://" + host + ":" + port + "/api/value-create", data = my_json_string, headers = header)    
    #r = requests.post('http://localhost:8000/api/value-create', data = my_json_string, headers = header, verify = False)
    #r = requests.post('http://192.168.188.68:8000/api/value-create', data = my_json_string, headers = header, verify = False)
    #r = requests.post('http://192.168.188.68:8000/api/value-create', data = my_json_string, headers = header, verify = False)
    r = requests.post('http://192.168.188.145:8000/api/value-create', data = my_json_string, headers = header, verify = False)
    #r = requests.post('http://192.168.0.220:8000/api/value-create', data = my_json_string, headers = header, verify = False)

#def postToServer():

if __name__ == '__main__':
    try:
        i = 0
        while True:
            generateNumber(i)
            #time.sleep(0.5)
            i = i + 8
            if (i >= 360):
                i = 0

    except KeyboardInterrupt:
        print('Interrupted')
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)        
