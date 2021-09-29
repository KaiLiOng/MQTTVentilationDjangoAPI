#include <ArduinoJson.h>
//#include <WiFi.h>
//#include <PubSubClient.h>
#include <EspMQTTClient.h>

 

const char* broker = "broker.hivemq.com";  // MQTT Broker server ip
const char* ssid = "FRITZ!Box 7590 UP";
const char* password = "45525509900873963179";
const char* clientName = "CoronaVentilator";
//const char* mqttport = 8000;
//
//  EspMQTTClient(
//    const char* wifiSsid = "FRITZ!Box 7590 UP",
//    const char* wifiPassword = "45525509900873963179",
//    const char* mqttServerIp = "broker.hivemq.com",
//    const char* mqttUsername,  // Omit this parameter to disable MQTT authentification
//    const char* mqttPassword,  // Omit this parameter to disable MQTT authentification
//    const char* mqttClientName = "CoronaVentilator",
//    const short mqttServerPort = 8000);
 

EspMQTTClient *client = new EspMQTTClient(ssid, password, broker, clientName);

 

long lastMsg =0;
char msg[50];
int value = 0;
float sinusnumber = 0;
int i = 0;

 

//WiFiClient espClient;
//PubSubClient client(espClient);

 

//StaticJsonDocument<200> doc;
const int capacitySensor = JSON_ARRAY_SIZE(4) + 4 * JSON_OBJECT_SIZE(4);
DynamicJsonDocument sensorDataJson(capacitySensor);
String JSONoutput;

 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  client->enableDebuggingMessages();
  client->setKeepAlive(60); // Timeout 1 minute
//  setup_wifi();
//  client.setServer(mqttServer,mqttPort);
//  client.setCallback(callback);
}

void onConnectionEstablished()
{
  while(1){
  //for (int j=0;j<720;j++){
    sinusnumber = sin(i*0.0174533);
    float a = roundf(sinusnumber*1000.0)/1000.0;
    i = i + 8;
    
    if (i >= 360)
    {
      i = 0;
    }

 

    // Convert the value to a char array
    JsonObject doc = sensorDataJson.createNestedObject();
    doc["ADR"] = 2;
    doc["BAT"] = a;
    serializeJson(sensorDataJson, JSONoutput);
    //(Serial.println(JSONoutput);
    sensorDataJson.clear();

 

    client->publish("CoronaVentilator/State", JSONoutput);
    JSONoutput.clear();
    //delay(0.5);
    }
} 

//void setup_wifi(){
//  delay(10);
//  Serial.println();
//  Serial.print("Connecting to ");
//  Serial.println(ssid);
//  
//  WiFi.begin(ssid, password);
//
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//
//  Serial.println("");
//  Serial.println("WiFi connected");
//}

 

//void callback(char* topic, byte* message, unsigned int length) {
//  Serial.print("Message arrived on topic: ");
//  Serial.print(topic);
//  Serial.print(". Message: ");
//  String messageTemp;
//  
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)message[i]);
//    messageTemp += (char)message[i];
//  }
//  Serial.println();
//
//  // Feel free to add more if statements to control more GPIOs with MQTT
//
//  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
//  // Changes the output state according to the message
//  if (String(topic) == "esp32/output") {
//    Serial.print("Changing output to ");
//    if(messageTemp == "on"){
//      Serial.println("on");
//    }
//    else if(messageTemp == "off"){
//      Serial.println("off");
//    }
//  }
//}
//
//void reconnect() {
//  // Loop until we're reconnected
//  while (!client.connected()) {
//    Serial.print("Attempting MQTT connection...");
//    // Attempt to connect
//    if (client.connect("CoronaVentilator")) {
//      Serial.println("connected");
//      // Subscribe
//      client.subscribe("esp32/output");
//    } else {
//      Serial.print("failed, rc=");
//      Serial.print(client.state());
//      Serial.println(" try again in 5 seconds");
//      // Wait 5 seconds before retrying
//      delay(5000);
//    }
//  }
//}

 

void loop() {
  // put your main code here, to run repeatedly:
//
//  if (!client.connected()) {
//    reconnect();
//  }
//  client.loop();
   client->loop();

 

//  long now = millis();
//  if (now - lastMsg > 5000) {
//    lastMsg = now;

 


  
//  }
}
