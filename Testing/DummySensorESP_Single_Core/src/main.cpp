/* Title: ESP-NOW and WiFi Connection to Client  ----- SensorESP -------
*
* Version: v1.3
* Autor: Jeremy Kunz
* Date: 06.10.2020
*
* Short Description:
*Sending Data between Multiply ESP32 via ESP-NOW and simultaneously connect to WiFi Network
*and sending Data to Client
*
* Output Data:
* Not output, just sending data
*
* Devices to connect:
* 
*
* Changelog:
* 28.09.2020 First Version
*  Title: ESP-NOW and WiFi Connection to Client  ----- SensorESP -------
*
* Version: v1.3
* Autor: Jeremy Kunz
* Date: 06.10.2020
*
* Short Description:
*Sending Data between Multiply ESP32 via ESP-NOW and simultaneously connect to WiFi Network
*and sending Data to Client
*
* Output Data:
* Sensor values & name of sensor
*
* Devices to connect:

*
* Short Description:
* Sensor ESP, try to include the Uart communication and the ESP Now connestion between multiple ESP
*
* Output Data:
* Sensor ESP sends Data to the MAster ESP (Sensorvalue,...)
*
* Devices to connect:
* 
*
* Changelog:
* 28.09.2020 First Version
* 06.10 2020 set WiFi Channel and ESP Now Channel
* 12.10.2020 Uart and ESP Now (v2.0)
* 14.10.2020 modifyed to use it for Module_2 (linear Axis)
*/
/*###########################################################################################################
 
            ----Declarations for Sensor-----
 
###########################################################################################################*/

// @TODO: Comment all Code and remove all unnecessary  code, change random generated code to real code from sensor

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define HIGHEST_CHANNEL 13 //Set according to country --> Germany 13
#define LOWEST_CHANNEL 1
#define FIXED_CHANNEL 6

/*###########################################################################################################
 * 
 * 
 *              -----VARIABLES AND JSON DECLARATION-----
 * 
 * 
  macAdresses for communication
*/
//Set the Mac adresses for all ESPs
uint8_t CommunicationMac[] = {0xA2, 0x46, 0x13, 0x46, 0x13, 0x46}; //first BIT can NOT be 1!
uint8_t alarmMac[] = {0x16, 0x64, 0x46, 0x79, 0x81, 0x96};
uint8_t sensorMac[] = {0x1A, 0x76, 0x56, 0x82, 0x71, 0x49};
uint8_t actorMac[] = {0x48, 0x63, 0x82, 0x91, 0x87, 0x14};
/*###########################################################################################################*/

esp_now_peer_info_t slave;
const esp_now_peer_info_t *slaveNode = &slave;
esp_err_t sendResult;
const uint8_t maxDataFrameSize = 200;
uint8_t dataToSend[maxDataFrameSize];
byte cnt = 0;
int dataSent = 0;

int ms_sleep = 0;

/*###########################################################################################################
 
            ----Datastructure sending-----
 
###########################################################################################################*/

typedef struct Sensor_Message_t
{
  int ESP_Identifier = 4; //1:Sensor 2:Actor 3:Alarm  4:Communication
  float Conc = 0;
  float Pres = 0;
  float InFlow = 0;
  float OutFlow = 0;
} Sensor_Message_t;

Sensor_Message_t Sensor_Message;

/*###########################################################################################################
 
            ----FUNCTIONS-----
 
###########################################################################################################*/

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  dataSent = (status == ESP_NOW_SEND_SUCCESS ? 1 : -1);
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  memcpy(&Sensor_Message, data, sizeof(Sensor_Message_t));

  esp_now_send(CommunicationMac, (uint8_t *)&Sensor_Message, sizeof(Sensor_Message_t));
}

//TestSend function to get the wifi and esp now on same channel
int TestSend()
{
  esp_err_t sendResult = esp_now_send(0, (uint8_t *)&Sensor_Message, sizeof(Sensor_Message));
  if (sendResult == ESP_OK)
  {
    Serial.println("Send success");
    return 1;
  }
  else if (sendResult == ESP_ERR_ESPNOW_NOT_INIT)
  {
    Serial.println("ESPNOW not Init.");
  }
  else if (sendResult == ESP_ERR_ESPNOW_ARG)
  {
    Serial.println("Invalid Argument");
  }
  else if (sendResult == ESP_ERR_ESPNOW_INTERNAL)
  {
    Serial.println("Internal Error");
  }
  else if (sendResult == ESP_ERR_ESPNOW_NO_MEM)
  {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  }
  else if (sendResult == ESP_ERR_ESPNOW_NOT_FOUND)
  {
    Serial.println("Peer not found.");
  }
  else if (sendResult == ESP_ERR_ESPNOW_IF)
  {
    Serial.println("Interface Error.");
  }
  else
  {
    Serial.printf("\r\nNot sure what happened\t%d", sendResult);
  }
  return 0;
}

void WiFiReset()
{
  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);
}

/*###########################################################################################################
 
            ----SETUP-----
 
###########################################################################################################*/

void setup()
{

  Serial.begin(115200);
  Serial.print("\r\n\r\n");

  WiFiReset();

  //iterate trough all channels and stop as soon as the correct channel is found
  for (uint8_t primaryChan = LOWEST_CHANNEL; primaryChan <= HIGHEST_CHANNEL; primaryChan++)
  {
    WiFi.mode(WIFI_STA);
    esp_wifi_set_mac(ESP_IF_WIFI_STA, &sensorMac[0]); //Mac adresse eintragen

    Serial.print("Current channel no: ");
    Serial.println(primaryChan);

    Serial.println(WiFi.macAddress());

    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    wifi_second_chan_t secondChan = WIFI_SECOND_CHAN_NONE;
    ESP_ERROR_CHECK(esp_wifi_set_channel(primaryChan, secondChan));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));

    WiFi.printDiag(Serial);

    WiFi.disconnect();

    Serial.print("New Wifi channel: ");
    Serial.println(WiFi.channel());

    if (esp_now_init() == ESP_OK)
    {
      Serial.println("ESP NOW INIT!");
    }
    else
    {
      Serial.println("ESP NOW INIT FAILED....");
    }

    slave.channel = primaryChan;
    slave.encrypt = 0;
    slave.ifidx = ESP_IF_WIFI_STA;

    memcpy(&slave.peer_addr, CommunicationMac, 6);
    if (esp_now_add_peer(slaveNode) == ESP_OK)
    {
      Serial.println("Added masterMac!");
    }
    memcpy(&slave.peer_addr, alarmMac, 6);
    if (esp_now_add_peer(slaveNode) == ESP_OK)
    {
      Serial.println("Added module 1");
    }
    memcpy(&slave.peer_addr, sensorMac, 6);
    if (esp_now_add_peer(slaveNode) == ESP_OK)
    {
      Serial.println("Added module 2");
    }
    memcpy(&slave.peer_addr, actorMac, 6);
    if (esp_now_add_peer(slaveNode) == ESP_OK)
    {
      Serial.println("Added module 3");
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    dataSent = 0;

    //Send test data
    if (TestSend())
    {

      //Wait for data sent confirmed
      for (int i = 0; i < 1000; i++, ms_sleep--)
      {
        delay(1);
        yield();
        if (dataSent != 0)
        {
          break; //No data sent, try another channel
        }
      }
      if (dataSent == 1)
      {
        //Save channel to nvr/RTC mem ?
        Serial.printf("Found Slave on channel: %d\n", WiFi.channel());
#ifndef USE_FIXED_CHANNEL
        break;
#endif
      }
    }
#ifndef USE_FIXED_CHANNEL
    delay(500);
  }
#endif

  /*###########################################################################################################
 
            ----Setup for SensorData-----
 
###########################################################################################################*/
}

/*###########################################################################################################
 
            ----LOOP-----
 
###########################################################################################################*/

void loop()
{
  Sensor_Message.InFlow = random(500, 2000);
  Sensor_Message.OutFlow = random(500, 2000);
  Sensor_Message.Pres = random(950, 1050);
  Sensor_Message.Conc = random(48, 52);
  esp_now_send(CommunicationMac, (uint8_t *)&Sensor_Message, sizeof(Sensor_Message_t));

  delay(500);
}
