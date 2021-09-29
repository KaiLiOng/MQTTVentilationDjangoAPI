/* Title: ESP-NOW and WiFi Connection to Server & first Implementation of Ventilator Functionality
*
* Version: v2.0
* Autor: Jeremy Kunz, Marcel Franz, Heiko Maier-Gerber
* Date: 13.10.2020
*
* Short Description:
* Sending Data between Multiple ESP32 via ESP-NOW and simultaneously connect to WiFi Network
* and sending Data to Client, also do the Ventilator chores.
* Now in two tasks on 2 cores, so code is qxecuted in parallel.
*
* Output Data:
* Recieved Values via ESP-NOW and Sensor/Actor Values, send to the Server
* Visualization of In/Exhaling via WS2812b LED Strip
* 
* Devices to connect:
* 2 other ESPs, that run the codes for SensorActor and Alarm. Each has to be set to a specific MAC address, see below
* LED Strip WS2812b, 100uF Cap is soldered in on beginning of Strip
* 
*
* Changelog:
* 28.09.2020 First Version - Jeremy Kunz
* 12.10.2020 cleared and repaired code, changed MAC Adresses suitable for Espressif given conditions in the ESP Documentation - Marcel Franz
* 13.10.2020 Implementing the Codes for Sensor, Actor, Alarm and Communication.
* 17.10.2020 JSON File is created with "doc" not with "buffer"
*            use ArduinoJson 6
*            Sending Data to client with client.print() function
*            not obj.printToClient()
* 21.10.2020 implemented changes of Heiko Maier-Gerber in this Sketch
* 27.10.2020 implemented newer changes of Heiko Maier-Gerber in this Sketch
* 08.11.2020 sending JSON File via Http Post to Python Flask server using ArduinoJSON HTTP            
* 15.11.2020 get Config JSON File from Python sever

* 25.11.2020 ##### Radical Change ##### it´s now DualCore (2Tasaks running on 2 cores)
* 25.11.2020 Implemented Inhaling/Exhaling animation, changed ESP-NOW Struct therefore
* 25.11.2020 Casted variables as int before they are sent to the server to see if this will fix the incorrectness in displaying
* 04.12.2020 Changed int values into float value and addes round2decimals() Function for using 2 decimals
* 10.12.2020 Cleaned up code, removed unused code and added comments
* 21.01.2021 Added a orange blinking to the LED stripe, as long as the WiFi is unconnected, followed by a green 3sec light tellin WiFi is connected
* 21.01.2021 HAVING WiFi OR ESP-NOW ISSUES? Then may it be because I changed the WiFi and ESP-NOW setup routine, as it here as well iterated through all the WiFi channels...
* 21.01.2021 Changed the send timing, so that Data is sent to the server everytime new Data comes in via ESP-NOW
*
* 25.02.2021 Changed the JSON Object to fit the !!! NEW WEBSITE !!! from now on. No more support for old website!
*
*/


// WS2812b LED Strip information from Manufacturer:
// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections. (is soldered in)
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel. (naja)
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR. (not soldered in)
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V micro



//controller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED. (if you say so... works anyway)
// (Skipping these may work OK on your workbench but can fail in the field) (oh, well... it´s a prototype:D)


//################################################################//
//########################## Includes ############################//

//ESP-NOW and Server Communication
#include <HTTPClient.h>                                                                                   // used for post and get request // do NOT install ArduinoHTTPClient library!!! The HTTP Library is from 
                                                                                                          // Espressif and comes with the AZdel dev-v4 board you selected // if you have it installed, UNINSTALL IT! - Thanks:*
#include <esp_now.h>                                                                                      // used for ESP Now Communication
#include <WiFi.h>                                                                                         // needed for Wifi
#include <esp_wifi.h>                                                                                     // needed to enable ESP Wifi
#include <ArduinoJson.h>                                                                                  // enables using JSON files // load library 6.xx.x from Arduino

//WS2812B LED Control
#include <Adafruit_NeoPixel.h>                                                                            // used for LEDs // download from Library Manager

//################################################################//
//########################## Defines #############################//
//################################################################//

// WLAN defines
#define HIGHEST_CHANNEL 13                                                                                // Highest WLAN channel used in Germany
#define LOWEST_CHANNEL 1
#define FIXED_CHANNEL 6

// LED defines
#define LED_PIN    33                                                                                     // Which pin on the ESP32 is connected to the NeoPixels?
#define LED_COUNT 43                                                                                      // How many NeoPixels are attached to the ESP32?

float sinusnumber = 0;
float cosinusnumber = 0;
int i=0;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)



//################################################################//
//######################### Variables  ###########################//
//################################################################//

// You can access global variables from all the tasks. However you may want to 
// think of a read/write collision protection, like a Semaphore

int VentilationMode = 0;                                                                                  // saves recieved ventilator Mode from settings input from python server
bool WiFiConnectedFlag = false;                                                                           // Flag used to show with LED stripe, if the WiFi is connected or not
bool DataRecieveFlag = false;                                                                             // Flag used to show if Data was recieved
unsigned long Millismerker = 0;                                                                           // Variable  to store System Uptime in ms in for timing purpose of Data request from Server
///////////////////////////////////////////////////////////
///// Task1 related: ESP-NOW and Server communication /////
///////////////////////////////////////////////////////////

//create the Task handle
TaskHandle_t Task1;                                                                                       // Creating Task 1 which is for ESP-NOW and Server communication

// JSON File

const int capacitySensor = JSON_ARRAY_SIZE(4) + 4 * JSON_OBJECT_SIZE(4);                                  // Setting size of JSON used to send sensor values
DynamicJsonDocument sensorDataJson(capacitySensor);                                                       // creating DyanmicJSONDocument used for Sensor Data

const int capacityConfig = JSON_ARRAY_SIZE(4) + 4 * JSON_OBJECT_SIZE(4);                                  // Setting size of JSON used to receive Ventilator Setting data from python server
DynamicJsonDocument configDataJSON(capacityConfig);                                                       // creating DyanmicJSONDocument used for Ventilator Settings                                            

String JSONoutput;                                                                                        // Output String which will contain sensor values as string and will be postet via post method to client server

#define PatientNumber 2                                                                                   // Patient Number for the Database to sort data to right patient

/////////////////////////////////////////////////
/////////// Task2 related: LED Control //////////
/////////////////////////////////////////////////

//create the Task handle
TaskHandle_t Task2;                                                                                       // Creating Task 2 which is for LED Control 

////////////////////////////////////////////
/// Struct for Data exchange via ESP-NOW ///
////////////////////////////////////////////

//Structure to send data via ESP-NOW. Must match the receiver structure
typedef struct Sensor_Message_t                                                                           // Sensor Data Struct
{
  int ESP_Identifier = 4;                                                                                 // 1:SensorActor 3:Alarm  4:Communication
  float Conc = 0;                                                                                         // Air Composition
  float Pres = 0;                                                                                         // Air Pressure
  float InFlow = 0;                                                                                       // Input / Inhalation Airflow
  float OutFlow = 0;                                                                                      // Output / Exhalation Airflow
  int LightControl = 0;                                                                                   // Variable to Control the LED Stripe (used for changing color for exhalation and inhalation)
} Sensor_Message_t;

Sensor_Message_t Sensor_Message;                                                                          // create Message of struct type


////////////////////////////////////////////
//////// ESP-NOW specific Variables ////////
////////////////////////////////////////////

///  Set the Mac adresses for all ESPs ///
uint8_t CommunicationMac[] = {0xA2, 0x46, 0x13, 0x46, 0x13, 0x45};                                        // first BIT can NOT be 1!
uint8_t alarmMac[] = {0x16, 0x64, 0x46, 0x79, 0x81, 0x96};                                                // MAC Adresses have to fit the Adresses from Sensor/Actor
uint8_t sensoractorMac[] = {0x1A, 0x76, 0x56, 0x82, 0x71, 0x49};
// uint8_t actorMac[] = {0x48, 0x63, 0x82, 0x91, 0x87, 0x14};

/// ESP NOW ///
esp_now_peer_info_t slave;                                                                                // info variable
const esp_now_peer_info_t *slaveNode = &slave;                                                            // pointer of info variable pointing on slave
esp_err_t sendResult;                                                                                     // result variable for adding peer (used in loop when ESP network is set up)


///////////////////////////////////////////////
///// WLAN Network - Connection Webserver /////
///////////////////////////////////////////////

/// WLAN Settings ///
const char *ssid = "FRITZ!Box 7590 UP";                                                                   // Enter SSID here
const char *password = "45525509900873963179";                                                            // Enter Password here

/// Connection to Webserver ///
// const String port = "8887";                                                                               // Set Port here // use unused Port and sychronize with python script
// const String host = "192.168.188.157";                                                                    // Webserver Device IP Adress

const String port = "8000";                                                                               // Set Port here // use unused Port and sychronize with python script
const String host = "192.168.188.157";  

//################################################################//
//######################### Functions  ###########################//
//################################################################//

/////////////////////////////////////////
///// Decleration of Task Functions /////
/////////////////////////////////////////

void Task1code(void *pvParameters);                                                                        // decleration of function that runs Task1 code // function is defined under the loop()
void Task2code(void *pvParameters);                                                                        // decleration of function that runs Task2 code // function is defined under the loop()


////////////////////////////////////////////
///// Functions for Calculating Values /////
////////////////////////////////////////////

/// Round and cut of 2 Decimals ///
float round2decimals (float var)                                                                          // cuts of and rounds float value on second decimals
{
//float a = roundf(var*1000.0)/1000.0;
return var;                                                                                                           // needed for correct process when values are recieved in python
//float value = (int)(var * 100 + 0.5);
//return (float)value/100;
}


///////////////////////////////////////////////////////////
//////// Functions Task1: ESP-NOW and Communication ///////
///////////////////////////////////////////////////////////

/// Function for printing Mac Adress ///
void prntmac(const uint8_t *mac_addr)                                                                     // Parameter is the mac adress array containing 6 parts
{
  Serial.print("MAC Address: {0x");
  for (byte i = 0; i < 6; ++i)                                                                            // loop through all 6 parts
  {
    Serial.print(mac_addr[i], HEX);
    if (i < 5)                                                                                            // after the first five parts print ","
      Serial.print(",0x");
  }
  Serial.println("};");                                                                                   
}

/// Data Received Callback ///
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)                            // ESP Now Callback function uses fix parameters
{
  memcpy(&Sensor_Message, incomingData, sizeof(Sensor_Message));                                          // copies received incomingData into Sensor_Message struct (destination, source, size)
  DataRecieveFlag = true;                                                                                 // mark the Data recieve flag, if it is true, the new dataset will be sent to the server
  Serial.print("Recieved struct, Pressure is: ");
  Serial.println(Sensor_Message.Pres,3);                                                                  // print Pressure Value with 3 decimals
  Serial.print("Recieved struct, Conc is: ");
  Serial.println(Sensor_Message.Conc,3);                                                                  // print Composition Value with 3 decimals
  Serial.print("Recieved struct, OutFlow is: ");
  Serial.println(Sensor_Message.OutFlow,3);                                                               // print Output Airflow Value with 3 decimals
  Serial.print("Recieved struct, InFlow is: ");
  Serial.println(Sensor_Message.InFlow,3);                                                                // print Input Airflow Value with 3 decimals
  Serial.print("Recieved struct, LED Control is: ");
  Serial.println(Sensor_Message.LightControl);                                                            // print LED Status Value
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)                                    // ESP Now Callback function uses fix parameters
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");                  // if sending was sucessfull status will get "Delivery Sucess"
  Serial.print("Data sent to ");
  prntmac(mac_addr);                                                                                      // call print function to print MAC Adress data sent to
}

/// Function for reseting WiFi ///
void WiFiReset()                                                                                          // later used in ESP & Communication Task 
{
  WiFi.persistent(false);                                                                                 // only changes current in-memory Wi-Fi settings,  does not affect  Wi-Fi settings stored in flash memory
  WiFi.disconnect();                                                                                      // Disconnection from current network
  WiFi.mode(WIFI_OFF);                                                                                    // WiFi is turned off
}

/// Creating JSON file ///
void createJSONfile()                                                                                     // creating one nested array with SensorData and ESP_Identifier
{
    //Sensor_Message.InFlow = random(100, 500);
  sinusnumber = sin(i*0.0174533);
  float a = roundf(sinusnumber*1000.0)/1000.0;

  cosinusnumber = cos(i*0.0174533);
  float b = roundf(cosinusnumber*1000.0)/1000.0;
  
  Sensor_Message.InFlow = a;
  Sensor_Message.OutFlow = b;

  Sensor_Message.Pres = 50;
  Sensor_Message.Conc = 100;
  
  i = i + 8;

  if (i >= 360)
  {
    i = 0;
  }

  //delay(15);

  JsonObject doc_0 = sensorDataJson.createNestedObject();
  doc_0["value"] = Sensor_Message.Pres;
  doc_0["sensor"] = "Pressure";
  doc_0["patient"] = PatientNumber;

  JsonObject doc_1 = sensorDataJson.createNestedObject();
  doc_1["value"] = Sensor_Message.Conc;
  doc_1["sensor"] = "Composition";
  doc_1["patient"] = PatientNumber;

  JsonObject doc_2 = sensorDataJson.createNestedObject();
  doc_2["value"] = Sensor_Message.InFlow;
  doc_2["sensor"] = "InFlow";
  doc_2["patient"] = PatientNumber;
  
  
  JsonObject doc_3 = sensorDataJson.createNestedObject();
  doc_3["value"] = Sensor_Message.OutFlow;
  doc_3["sensor"] = "OutFlow";
  doc_3["patient"] = PatientNumber;

  //serializeJsonPretty(sensorDataJson, Serial);
  serializeJson(sensorDataJson, JSONoutput);
  Serial.println(JSONoutput);
  sensorDataJson.clear();
}

/// Function for sending data ///
bool sendDataToServer(String mServerIP, String mPort, String requestLink)                                 // Parameters are the Server IP Adress and the Port, requestLink is not used here
{
  HTTPClient http;                                                                                        // HTTP object needed for http post method
  http.begin("http://" + mServerIP + ":" + mPort + "/api/value-create");                                              // set URL path and http method
  http.addHeader("Content-Type", "application/json");                                                     // define a header for the request specifying content-type as app/json
  createJSONfile();                                                                                       // create String "JSON file" with current sensor values
  int httpResponseCode = http.POST(JSONoutput);                                                           // String "JSON file" is sent via HTTP Post method to Python server // httpResponseCode safes the return value
  JSONoutput = "";                                                                                        // Clearing the string after sending to ensure sending a consistent message
  if (httpResponseCode != 200)                                                                            // HTTP_Status_Code 200 means HTTP_OK                                       
  {
    Serial.print("Http status code: ");
    Serial.println(httpResponseCode);                                                                     // Print status code when is was not sucessfull
    return false;                                                                                         // no sucess
  } 
  http.end();                                                                                             // close TCP connection and free resources
  return true;
}

/// Function for receiving data ///
bool getDataFromServer(String mServerIP, String mPort, String requestLink)                                // Parameters: Server IP Adress, Port, requestLink has to be same phrase used in python to send data to ESP via get request
{
  HTTPClient http;                                                                                        // HTTP object needed for http get method
  http.useHTTP10(true);                                                                                   // HTTP10 has to be set to use getStream() method
  http.begin("http://" + mServerIP + ":" + mPort + "/" + requestLink);                                    // set URL path and http method
  http.addHeader("Content-Type", "application/json");                                                     // define a header for the request specifying content-type as app/json
  int httpResponseCode = http.GET();                                                                      // safe get() method response code
  if (httpResponseCode != 200)                                                                            // check if get() has worked // HTTP_Status_Code 200 means HTTP_OK
  {
    Serial.print("Http status code: ");
    Serial.println(httpResponseCode);                                                                     // Print status code when is was not sucessfull
    return false;                                                                                         // no sucess
  }
  deserializeJson(configDataJSON, http.getStream());                                                      // getStream() is more efficient than getString() // JSON input is parsed into JsonDocument configDataJSON
  Serial.print("Recieved Data from Server: ");
  VentilationMode = configDataJSON["modus"].as<bool>();                                                   // Save received Data from Server in global variable // looking for "modus" in JSON file
  Serial.println(VentilationMode);                                                                        // Test print of Ventilation Mode
  http.end();                                                                                             // close TCP connection and free resources
  return true;
}


//////////////////////////////////////////////
//////// Functions Task2: LED Control ////////
//////////////////////////////////////////////

/// Creating animated effects ///

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. 
// Functions are called from while(true) loop in task 2 for LED Control

/// Wipe first direction ///
void colorWipe(uint32_t color, int wait)                                                                  // parameters: 1) single 'packed' 32-bit value, which is get by calling strip.Color(red, green, blue), 2) delay time (in milliseconds) between pixels
{
  for(int i=0; i<strip.numPixels(); i++)                                                                  // For each pixel in the strip
  {
    strip.setPixelColor(i, color);                                                                        //  Set pixel's color (in RAM)
    strip.show();                                                                                         //  Update strip to match
    delay(wait);                                                                                          //  Pause for a moment
  }
}

/// Wipe opposite direction ///
void colorWipeReverse(uint32_t color, int wait)                                                           // parameters: 1) single 'packed' 32-bit value, which is get by calling strip.Color(red, green, blue), 2) delay time (in milliseconds) between pixels
{  
  for(int i=(strip.numPixels()-1); i>=0; i--)                                                             // For each pixel in strip // reverse for loop
  {
    strip.setPixelColor(i, color);                                                                        //  Set pixel's color (in RAM)
    strip.show();                                                                                         //  Update strip to match
    delay(wait);                                                                                          //  Pause for a moment
  }
}


//################################################################//
//########################### SETUP ##############################//
//################################################################//

// the setup function runs once when you press reset or power the board, it runs on core1
void setup() {
  
  Serial.begin(115200);                                                                                   // initialize serial communication at 115200 bits per second:

  /// Now set up two tasks to run independently and additionaly on different cores ///

  xTaskCreatePinnedToCore(
        Task1code,                                                                                        // Task 1 is ESP Now & Communication Task
        "Task1",                                                                                          // name of task
        10000,                                                                                            // Stack memory size of task
        NULL,                                                                                             // parameter of the task
        1,                                                                                                // priority of the task, 0=lowest
        &Task1,                                                                                           // Task handle to keep track of created task
        1                                                                                                 // pin task to core 1 
  );

  // xTaskCreatePinnedToCore(
  //       Task2code,                                                                                        // Task 2 is LED Control
  //       "Task2",                                                                                          // name of task
  //       10000,                                                                                            // Stack size of task
  //       NULL,                                                                                             // parameter of the task
  //       1,                                                                                                // priority of the task, 0=lowest
  //       &Task2,                                                                                           // Task handle to keep track of created task
  //       0                                                                                                 // pin task to core 0
  // );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}


//################################################################//
//############################ Loop  #############################//
//################################################################//

void loop()
{
  /* Empty. Things are done in Tasks. If Code is put here, it will 
    be treatened as another task on core 1. But just don´t 
    do it this way. Use tasks!*/
}

//################################################################//
//############################ TASKS  #############################//
//################################################################//

//////////////////////////////////////
/////////////// TASK 1 ///////////////
//////////////////////////////////////

/// This is the ESP Now & Communication Task on Core 1 ///
void Task1code(void *pvParameters)  
{
   /*Here, in the task and before the while(true) you can place 
  Startup Code for this task, it runs once when created */

  (void) pvParameters;                                                                                      // needed to run task 
  WiFiReset();                                                                                              // run deactivation of Wifi
  //esp_base_mac_addr_set(&CommunicationMac[0]);                                                              // change Mac adress HERE, had some issues with it being set elswhere // set to first part of Communication MAC
  WiFiClient client;                                                                                        // Creating client that can connect to  specified internet IP address and port 
              
  WiFi.mode(WIFI_AP_STA);                                                                                   // Configure WiFi as Access Point and Station For connection to Router(Server) and for ESP-NOW
  //esp_wifi_set_mac(ESP_IF_WIFI_STA, &CommunicationMac[0]);                                                  // ESP Wifi Station Mac Adress is set to first part of Communication MAC                                 
  Serial.println(WiFi.macAddress());                                                                        // Print Wifi MAC Adress

    /// Connect to WiFi network ///
  Serial.print("Connecting to ");
  Serial.println(ssid);                                                                                     // again Control Print of Network Name                                                                  
  WiFi.begin(ssid, password);                                                                               // Initialize WiFi Connection // Parameters: Network Name, Password
  while (WiFi.status() != WL_CONNECTED){                                                                    // Check Wifi Status for sucessfull connection
    delay(500);
    Serial.print(".");                                                                                      // "Waiting" print until connection is sucessfull
  }

  WiFiConnectedFlag = true;                                                                                 // set the flag true, used to show with LED stripe, if the WiFi is connected or not
  Serial.print("IP Adress: ");
  Serial.println(WiFi.localIP());                                                                           // Print received IP Adress of ESP32 in the network

  WiFi.printDiag(Serial);       
  
  /*
  if (esp_now_init() == ESP_OK){                                                                            // Initialize ESP Now and check for sucess
    Serial.println("ESP-NOW initialized! :) ");                                                             // lucky you!
  } else {
    Serial.println("ESP NOW initialization FAILED :( ");                                                    // Ooops
  }
              
  slave.channel = 0;                                                                                      // Slave Channel gets current channel of the for loop //Wi-Fi channel that peer uses to send/receive ESPNOW data. If the value is 0, use the current channel which station or softap is on. Otherwise, it must be set as the channel that station or softap is on.
  slave.encrypt = 0;                                                                                      // no encryption used
  slave.ifidx = ESP_IF_WIFI_STA;                                                                          // needed to run ESP Now

  /// Add ESP-NOW Peers ///          
  memcpy(&slave.peer_addr, alarmMac, 6);                                                                  // copies AlarmMAC Adress to Slave Peer Adress (destination, source, size)
  if (esp_now_add_peer(slaveNode) == ESP_OK)                                                              // Functions adds device to paired devices list
  {
    Serial.println("Added peer to Alarm ESP");                                                            // if adding was sucessfull state sucess
  }
  memcpy(&slave.peer_addr, sensoractorMac, 6);                                                            // copies SensorMAC Adress to Slave Peer Adress (destination, source, size)
  if (esp_now_add_peer(slaveNode) == ESP_OK)                                                              // Functions adds device to paired devices list
  {
    Serial.println("Added peer to Sensor-Actor ESP\n");                                                   // if adding was sucessfull state sucess
  }
  

  /// Register ESP-NOW Callbacks ///            
  esp_now_register_send_cb(OnDataSent);                                                                   // register sending callback function                                                                                                                                                               // register for each channel ???
  esp_now_register_recv_cb(OnDataRecv);                                                                   // register receiving callback function
  */
 
/*
                                                                                                                                                                                                                                                                                                                  // WHY IS THIS PERFORMED ON THE COMMUNICATION ESP????
    /// Set ESP Now Connection for each channel ///
  for (uint8_t primaryChan = LOWEST_CHANNEL; primaryChan <= HIGHEST_CHANNEL; primaryChan++)                 // iterate trough all channels                                                                                                                                                                        // and stop as soon as the correct channel is found ???
  {
    WiFi.mode(WIFI_AP_STA);                                                                                 // Configure WiFi as Access Point and Station For connection to Router(Server) and for ESP-NOW
    esp_wifi_set_mac(ESP_IF_WIFI_STA, &CommunicationMac[0]);                                                // ESP Wifi Station Mac Adress is set to first part of Communication MAC                                 
    Serial.print("Current channel no: ");
    Serial.println(primaryChan);                                                                            // control print current channel number
              
    Serial.println(WiFi.macAddress());                                                                      // Print Wifi MAC Adress
              
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));                                                        // if ESP_ERROR_CHECK is not equal ESP_OK, a error message is printed automatically // set_promiscuousset() enables the promiscuous mode
    wifi_second_chan_t secondChan = WIFI_SECOND_CHAN_NONE;                                                  // sets the channel width to HT20 
    ESP_ERROR_CHECK(esp_wifi_set_channel(primaryChan, secondChan));                                         // Set primary/secondary channel of ESP32
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));                                                       // disables the promiscuous mode

    WiFi.printDiag(Serial);                                                                                 // print out diagnostic information to Serial          
    WiFi.disconnect();                                                                                      // Disconnection from current network
              
    Serial.print("New Wifi channel: ");
    Serial.println(WiFi.channel());                                                                         // print Channel
              
    if (esp_now_init() == ESP_OK)                                                                           // Initialize ESP Now and check for sucess
    {
      Serial.println("ESP NOW INIT!");                                                                      // confirm Sucess
    }
    else
    {
      Serial.println("ESP NOW INIT FAILED....");                                                            // negative sucess
    }
              
    slave.channel = primaryChan;                                                                            // Slave Channel gets current channel of the for loop
    slave.encrypt = 0;                                                                                      // no encryption used
    slave.ifidx = ESP_IF_WIFI_STA;                                                                          // needed to run ESP Now

     /// Add Peers ///          
    memcpy(&slave.peer_addr, CommunicationMac, 6);                                                          // copies CommMAC Adress to Slave Peer Adress (destination, source, size)
    if (esp_now_add_peer(slaveNode) == ESP_OK)                                                              // Functions adds device to paired devices list // slaveNode is a pointer on esp_now_peer_info_t "slave"
    {
      Serial.println("Added masterMac!");                                                                   // if adding was sucessfull state sucess
    }
    memcpy(&slave.peer_addr, alarmMac, 6);                                                                  // copies AlarmMAC Adress to Slave Peer Adress (destination, source, size)
    if (esp_now_add_peer(slaveNode) == ESP_OK)                                                              // Functions adds device to paired devices list
    {
      Serial.println("Added module 1");                                                                     // if adding was sucessfull state sucess
    }
    memcpy(&slave.peer_addr, sensorMac, 6);                                                                 // copies SensorMAC Adress to Slave Peer Adress (destination, source, size)
    if (esp_now_add_peer(slaveNode) == ESP_OK)                                                              // Functions adds device to paired devices list
    {
      Serial.println("Added module 2");                                                                     // if adding was sucessfull state sucess
    }
    memcpy(&slave.peer_addr, actorMac, 6);                                                                  // copies ActorMAC Adress to Slave Peer Adress (destination, source, size)
    if (esp_now_add_peer(slaveNode) == ESP_OK)                                                              // Functions adds device to paired devices list
    {
      Serial.println("Added module 3");                                                                     // if adding was sucessfull state sucess
    }

    /// Register ESP NOW Callbacks ///            
    esp_now_register_send_cb(OnDataSent);                                                                   // register sending callback function                                                                                                                                                               // register for each channel ???
    esp_now_register_recv_cb(OnDataRecv);                                                                   // register receiving callback function
  }
  */
              

  //////////////////////////////
  //// main loop for Task 1 ////

  while(true)                                                                                              // a task never should loop out or return!!
  {
    //if(DataRecieveFlag == true){
    //  DataRecieveFlag = false;
      
      if (sendDataToServer(host, port, "post")){                                                          // Call Sending Function to send current sensor values to webserver
        Serial.println("Sending successful");  
        //delay(0.5);
                                                                   // state sucess when true was returned
      } else {
        Serial.println("Sending not successful");                                                         // state failure when false was returned
      }
    //}
    // if(abs(millis()-Millismerker) >= 500){                                                                // wait untill next data request from Server
    //   Millismerker = millis();
    //   getDataFromServer(host, port, "getConfig");                                                         // Call Receiving Function to safe Vantilation Settings from Web Server
    // }                                                                           
  }
}


//////////////////////////////////////
/////////////// TASK 2 ///////////////
//////////////////////////////////////

/// This is the LED control Task on Core 0 ///
void Task2code(void *pvParameters)  
{
  /*Here, in the task and before the while(true) loop you can place 
  Startup Code for this task, it runs once when created */

   (void) pvParameters;                                                                                    // needed to run task

  // /// LED Strip Specific ///
  // int Rin = 0;                                                                                            // Red Inhalation Color
  // int Gin = 0;                                                                                            // Green Inhalation Color
  // int Bin = 0;                                                                                            // Blue Inhalation Color
  // int Rout = 0;                                                                                           // Red Exhalation Color 
  // int Gout = 0;                                                                                           // Green Exhalation Color 
  // int Bout = 0;                                                                                           // Blue Exhalation Color
              
  // strip.begin();                                                                                          // INITIALIZE NeoPixel strip object (REQUIRED)
  // strip.show();                                                                                           // Turn OFF all pixels as soon as possible

  // while(WiFiConnectedFlag == false){                                                                      // flash the LED stripe while the WiFi is not connected
  //   strip.fill(strip.Color(255,70,  0));                                                                 // set every pixel to the color: orange
  //   strip.show();                                                                                         // show on LED stripe
  //   vTaskDelay(200);

  //   strip.clear();                                                                                        // set every pixel to the color: off
  //   strip.show();                                                                                         // show on LED stripe
  //   vTaskDelay(200);
  // }       

  // strip.fill(strip.Color(  0,255,  0));                                                                   // show green light to tell WiFi is connected
  // strip.show();
  // vTaskDelay(3000);
  // strip.clear();                                                                                          // do a little break before starting the light show
  // strip.show();
  // vTaskDelay(1000);


  // //////////////////////////////
  // //// main loop for Task 2 ////  

   while(true)                                                                                             // a task never should loop out or return!!
   {
  //   /// change Colour depending of Ventilation Mode set on Webapp //
  //   if(VentilationMode == 0)                                                                              // Check if Ventilation Mode is "CPR Mode"
  //   {
  //     Rin  =   0; Gin  =   0; Bin  = 255;                                                                 // Set Inhalation RGB Colors
  //     Rout = 255; Gout =   0; Bout =  10;                                                                 // Set Exhalation RGB Colors
  //   }
  //   if(VentilationMode == 1)                                                                              // Check if Ventilation Mode is "PRVC Mode"
  //   {
  //     Rin  = 255; Gin  = 200; Bin  = 150;                                                                 // Set Inhalation RGB Colors
  //     Rout =   0; Gout = 255; Bout =   0;                                                                 // Set Exhalation RGB Colors
  //   }
              
  //   switch(Sensor_Message.LightControl)                                                                   // Check for LightControl Value of Sensor Value struct received via ESP Now
  //   {
  //     case 0:                                                                                             // Inhalation
  //       /// Fill along the length of the strip in various colors ///
  //       colorWipeReverse(strip.Color(Rin, Gin, Bin), 5);                                                  // Call colorWipeReverse() function // parameters: 32bit color value (returned by strip.Color()) and delay time 5ms
  //       colorWipeReverse(strip.Color(  0,   0,   0), 5);                                                  // turn off with delay time 5ms
  //       break;
  //     case 1:                                                                                             // Exhalation
  //       /// Fill along the length of the strip in various colors ///
  //       colorWipe(strip.Color(Rout, Gout, Bout), 5);                                                      // Call colorWipeReverse() function // parameters: 32bit color value (returned by strip.Color()) and delay time 5ms
  //       colorWipe(strip.Color(  0,   0,   0), 5);                                                         // turn off with delay time 5ms
  //       break;
  //     default:                                                                                            // In case of other values
  //       strip.clear();                                                                                    // turn of stripe 
  //       strip.show();                                                                                     // turn of all pixels
  //       break;
  //   }           
   }
}