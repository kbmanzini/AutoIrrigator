#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <GeeekPiMQTT.h>

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "PHICOMM";
const char* password = "HomeLabsTestNetwork";

// Setting ESP8266 Serial Port(TX-P2, RX-P3)
SoftwareSerial ESP8266(2, 3);

// Initializes the espClient.
WiFiEspClient espClient;
GeeekPiMQTT MQTTClient(espClient);

// WiFi Status
int WiFi_Status = WL_IDLE_STATUS;

String topic = "button/led1";

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

// This functions connects your ESP8266 to your router
void setup_wifi() {
  //Setting ESP8266 Baud Rate(9600bps)
  ESP8266.begin(115200 );

  //Init ESP8266
  WiFi.init(&ESP8266);

  Serial.print("Processing WiFi Setting!\r\n");
  do {
    Serial.println("WiFi Connecting ...");
    WiFi_Status = WiFi.begin(ssid, password);
    delay(500);
  } while (WiFi_Status != WL_CONNECTED);

  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.println("WiFi Setting Done");
}

// Subscribe Message Callback Function(if necessary)
void message_callback(String topic, String message) {
  Serial.print("Topic: ");
  Serial.print(topic);
  Serial.print("; Message:");
  Serial.println(message);
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));

  // MakerCloudMQTT Configuration
  MQTTClient.setServer("172.84.93.181",1883);
  // Enable to print extra log
  MQTTClient.setLog(true);

  // Connect ESP8266 to router
  setup_wifi();

  // Set callback function with different data type if need to subscribe topics
  MQTTClient.setMessageCallback(message_callback);

  // Connect to MakerCloud
  if (MQTTClient.connect("arduino_0001","taterli","xxooxxoo",0,0,0,0,1)) {
    // Subscribe to a topic
    MQTTClient.subscribe(topic);
  }
}

// The looping function will allow sending random number to MakerCloud
void loop() {
  // Update user status and process 1 incoming MQTT message
  now = millis();
  // Publishes new random number every 5 seconds
  if (now - lastMeasure > 1000) {
    MQTTClient.listen();
    lastMeasure = now;
       // Publish Message
       //MQTTClient.publish_message(topic, "hello from arduino");
  }
}
