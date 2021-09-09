<h1> Ventilation Django API & Website </h1>
<p>New version of the Ventilation Page written in Python with the Framework <a href="https://www.djangoproject.com">Django</a>
    & <a href="https://www.django-rest-framework.org">Django Rest Framework</a>
</p>

<h2>Description</h2>
<p>Completly new web application of the old web page, with database connection and a API for handling incoming data from different ESP32 (Patients).
    New Improvements:
    <ul>
        <li>Registration and Login page</li>
        <li>Search Function</li>
        <li>Faster refresh rate</li>
        <li>Admin page for doctor and super user</li>
    </ul>   
</p>

<h2>Table of Contents</h2>
<ol>
    <li><a href="#installation">Installation</a></li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#credits">Credits</a></li>
    <li><a href="#license">License</a></li>
  </ol>

  <h2 id="installation">Installation</h2>
    <h3>Installation via Git Commands</h3>
        <p>
            To install the ventilation page via Git commands, you should first create a folder in your prefered directory.
            Open your git bash, CMD or terminal in the specified folder. Then typ in:</br> </br>  
            <kbd>
                    git clone https://github.com/DevTeamVenti/VentilationDjangoAPI.git
            </kbd>
            </br>
            </br>
            Now you folder should contain the git project.
        </p>
    <h3>Installation via ZIP file </h3>
        <p> 
            Download the project under code --> Download ZIP
            Save the zip in your prefered folder and unzip it.
        </p>
    <h3>Getting Started</h3>
        <p> Open the folder on your prefered IDE (normally <a href="https://code.visualstudio.com">Visual Studio Code</a>).
            Now typ in the follwing commands in your IDE Terminal.</p>
        </br>
        </br>
        <strong><em>Python should already installed on your machine, if not download python <a href="https://www.python.org">here</a>, and allow python to install the path variable. Also be sure ypu have installed python in VS Code</em></strong> 
        <p>You can check if python is working with </p>    
        <kbd>
            python --verison
        </kbd>
        </br>
        </br>
        Now you have to install a required packages:
        For more Information see </br>
        <a href="https://www.django-rest-framework.org">Django REST Framework</a>
        and <a href="https://docs.djangoproject.com/en/3.1/topics/install/">Django</a>
        </br>
        Write the following commands in your VS Code console:
        </br>
        <ul>
            <li>
                <kbd>
                    python -m pip install Django
                </kbd>
            </li>
            <li>
                <kbd>
                    pip install djangorestframework
                </kbd>
            </li>               
            <li>
                <kbd>
                    pip install markdown  
                </kbd>
            </li>
            <li>
                <kbd>
                    pip install django-filter 
                </kbd>
            </li>                    
        </ul> 
        <p>And finally:</p>
        <ul>
                <li>
                    <kbd>
                        python manage.py makemigrations
                    </kbd>
                </li>
                <li>
                    <kbd>
                        python manage.py migrate
                    </kbd>
                </li> 
                <li>
                    <kbd>
                        python manage.py migrate --run-syncdb
                    </kbd>
                </li> 
                <li>
                    <kbd>
                        python manage.py createsuperuser
                    </kbd>
                </li>
                <li>
                    <kbd>
                        python manage.py migrate
                    </kbd>
                </li>            
                <li>
                    <kbd>
                        python manage.py runserver 0.0.0.0:8080
                    </kbd>
                </li>              
            </ul> 
    <p>
        With these commands you initalize the database, api and also created a superuser with the password you typed in. At least you start the python server
        at the ip address of your own machine: </br>
        <a href="https://www.windows-commandline.com/find-ip-address/">How to get own ip adress</a> </br>       
        Now you can reach your page at: http://YOUR_IP:8080
    </p>

<h2 id="usage">Usage</h2>
    <p> Django is using the <a href="https://en.wikipedia.org/wiki/Model–view–controller">Model-view-controller</a> software design pattern</p>
     <h3>Adding new HTML Page</h3>
         All HTML pages are located in main/templates/main. Here you can add a new HTML page. Before you can call the new page you have to add the url in main/urls.py.
         The basic structure of a HTML site looks like:
         </br></br>
        {% extends "main/header.html" %}</br>
        {% block content %}</br></br>
         Your content</br></br>
        {% endblock %}</br></br>
    <h3>CSS & Javascript</h3>
        <p>         
            As frontend css framework we used <a href="http://getbootstrap.com">bootstrap</a>. It is implemented in main/templates/header.html.</br>
            All Javascript & CSS files are located in main/static. There is also the <a href="https://plotly.com/javascript/">plotly.min.js</a> which is the framework for plotting sensor values.
        </p>
    <h3>API</h3>
    <p>The api contains all models (patient, sensor, value) with all specified attributes. It also contains the serializers.py which save the incoming json data to the model.
    The api also handels the relationship between the different models.
    </p>
