#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include <string.h>
#include "arduino_secrets.h"


//please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

//define soil moisture sensor pin
#define humi1 A2
#define humi2 A1
#define humi3 A0 

// set variable
int status = WL_IDLE_STATUS;

// init wificlient
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// MQTT Broker infor
const char broker[] = "192.168.3.218";
int port = 1883;

// topic infor
const char topic1[] = "uno/soil_moisture_ch1";
const char topic2[] = "uno/soil_moisture_ch2";
const char topic3[] = "uno/soil_moisture_ch3";

void setup() {
  // Initialize serial port
  Serial.begin(9600);

  // attempt to connect to WiFi network
  Serial.print("attempt to connect to WPA SSID: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // if failed on connecting wifi, retry again.
    Serial.print(".");
    delay(5000);  // wait for 5 seconds and then retry to connect to wifi.
  }

  Serial.println("Connecting to wifi successful");
  Serial.println("More infor:");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  mqttClient.setId("Plant_watering_kit");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  mqttClient.setUsernamePassword("jacky", "mypassword");

  Serial.print("Attempting to connect to the MQTT broker: ");

  Serial.println(broker);

  while (!mqttClient.connect(broker, port)) {

    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // reading data from soil moisture
  float raw_data_moisture1 = analogRead(humi1); 
  float raw_data_moisture2 = analogRead(humi2);
  float raw_data_moisture3 = analogRead(humi3);

  // print on serial monitor 
  // Serial.print("Sending message to topic: ");
  // Serial.println(topic1);
  // Serial.print("Soil moisture raw data:");
  // Serial.println(100.0 - (raw_data_moisture / 10.24));

  // send meassage, the print interface can be used to set the message contents
  mqttClient.beginMessage(topic1);
  mqttClient.print(100.0 - (raw_data_moisture1 / 10.24));
  mqttClient.endMessage();

  mqttClient.beginMessage(topic2);
  mqttClient.print(100.0 - (raw_data_moisture2 / 10.24));
  mqttClient.endMessage();
  
  mqttClient.beginMessage(topic3);
  mqttClient.print(100.0 - (raw_data_moisture3 / 10.24));
  mqttClient.endMessage();

  delay(200); // sleep for a while 
  }
