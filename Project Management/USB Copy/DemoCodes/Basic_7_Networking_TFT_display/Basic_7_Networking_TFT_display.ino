#include "WiFiS3.h"
#include "arduino_secrets.h"

#include <TFT_eSPI.h>   // Hardware-specific library 
#include <TFT_eWidget.h>  //widget library 

char ssid[] = SECRET_SSID;  // your network SSDI(name)  
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

// define leds 
#define LED_OFFLINE 7
#define LED_ONLINE 6 

// define soil moisture sensor pin
#define humi1  A2

// define wifi status i
int status = WL_IDLE_STATUS; 

// create instance of TFT　display
TFT_eSPI tft = TFT_eSPI();

void setup() {
  // init serial port 
  Serial.begin(9600);

  // init leds 
  pinMode(LED_OFFLINE, OUTPUT);
  pinMode(LED_ONLINE, OUTPUT);

  // init tft display and clear screen
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(3);
  tft.println("Attempting to connect to wifi network!!!");
  tft.println("Please wait for a while....");
 
  digitalWrite(LED_OFFLINE, HIGH); // turn on Red led unless wifi is connected. 

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection;
    delay(10000);
    
  }
    digitalWrite(LED_OFFLINE, LOW); // turn off red led 
    digitalWrite(LED_ONLINE, HIGH); // turn on green led means that wifi connection is ok. 
}

void loop() {

   IPAddress ip = WiFi.localIP();
  //  Serial.print("IP address: ");
  //  Serial.println(ip);
   //reading soil moisture sensor's raw data. 
   float humi1_value = analogRead(humi1);

   tft.fillScreen(TFT_BLACK);
   tft.setCursor(0,0);
   tft.setTextColor(TFT_GREEN);
   tft.setTextSize(3);
   tft.println("IP address: ");
   tft.println(ip);
   tft.println("HUMI1 Rawdata:");
   tft.println(humi1_value);
   delay(3000);
}
