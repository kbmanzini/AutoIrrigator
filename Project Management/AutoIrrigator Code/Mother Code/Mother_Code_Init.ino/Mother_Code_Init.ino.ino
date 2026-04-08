#define BLYNK_TEMPLATE_ID "TMPL2VzRtGBQt"
#define BLYNK_TEMPLATE_NAME "Plant Watering Kit"
#define BLYNK_AUTH_TOKEN "n4ah771AAc91Iiswb1PT1zLeZPYghnbL"

#include <WiFiS3.h>
#include <BlynkSimpleWiFiS3.h>

// -------------------------
// WiFi Credentials
// -------------------------
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// -------------------------
// Hardware Pin Definitions
// -------------------------

// Soil moisture sensors
#define SOIL1_PIN A0
#define SOIL2_PIN A1
#define SOIL3_PIN A2

// Temperature sensors (adjust if your kit uses different pins)
#define TEMP1_PIN A3
#define TEMP2_PIN A4
#define TEMP3_PIN A5

// Pump control pins (adjust if your kit uses different pins)
#define PUMP1_PIN 7
#define PUMP2_PIN 8
#define PUMP3_PIN 9

// Optional status LEDs (change/remove if not used)
#define LED_OFFLINE 4
#define LED_ONLINE 5
#define LED_AUTO 6

// -------------------------
// Calibration Values
// IMPORTANT: You should test and adjust these!
// -------------------------
// These are EXAMPLE values for capacitive soil sensors.
// Replace after testing dry vs wet readings.
const int SOIL1_DRY = 850;
const int SOIL1_WET = 400;

const int SOIL2_DRY = 850;
const int SOIL2_WET = 400;

const int SOIL3_DRY = 850;
const int SOIL3_WET = 400;

// -------------------------
// Global Variables
// -------------------------

// Sensor values sent to Blynk
int soil1 = 0;
int soil2 = 0;
int soil3 = 0;

float temp1 = 0.0;
float temp2 = 0.0;
float temp3 = 0.0;

// Control values from Blynk
bool autoMode = true;
bool emergencyStop = false;

int threshold1 = 35;
int threshold2 = 35;
int threshold3 = 35;

int pumpDuration = 3000; // milliseconds (3 sec default)

// Pump state tracking
bool pump1Running = false;
bool pump2Running = false;
bool pump3Running = false;

unsigned long pump1StartTime = 0;
unsigned long pump2StartTime = 0;
unsigned long pump3StartTime = 0;

// Timers
BlynkTimer timer;

// -------------------------
// Helper Functions
// -------------------------

// Convert raw soil sensor value to 0-100% moisture
int readSoilPercent(int pin, int dryValue, int wetValue) {
  int raw = analogRead(pin);

  // Map raw sensor value to 0-100%
  int percent = map(raw, dryValue, wetValue, 0, 100);

  // Clamp to valid range
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;

  return percent;
}

// Example temp reading function
// If your kit uses analog temp sensors like LM35:
// LM35 = 10mV per degree C
float readTemperatureC(int pin) {
  int raw = analogRead(pin);

  // UNO R4 ADC reference is typically 5.0V
  float voltage = (raw / 1023.0) * 5.0;
  float tempC = voltage * 100.0; // LM35 formula

  return tempC;
}

// Pump control helpers
void startPump1() {
  if (!pump1Running && !emergencyStop) {
    digitalWrite(PUMP1_PIN, HIGH);
    pump1Running = true;
    pump1StartTime = millis();
    Blynk.virtualWrite(V1, 1); // update app switch/status
    Serial.println("Pump 1 STARTED");
  }
}

void stopPump1() {
  digitalWrite(PUMP1_PIN, LOW);
  pump1Running = false;
  Blynk.virtualWrite(V1, 0);
  Serial.println("Pump 1 STOPPED");
}

void startPump2() {
  if (!pump2Running && !emergencyStop) {
    digitalWrite(PUMP2_PIN, HIGH);
    pump2Running = true;
    pump2StartTime = millis();
    Blynk.virtualWrite(V2, 1);
    Serial.println("Pump 2 STARTED");
  }
}

void stopPump2() {
  digitalWrite(PUMP2_PIN, LOW);
  pump2Running = false;
  Blynk.virtualWrite(V2, 0);
  Serial.println("Pump 2 STOPPED");
}

void startPump3() {
  if (!pump3Running && !emergencyStop) {
    digitalWrite(PUMP3_PIN, HIGH);
    pump3Running = true;
    pump3StartTime = millis();
    Blynk.virtualWrite(V3, 1);
    Serial.println("Pump 3 STARTED");
  }
}

void stopPump3() {
  digitalWrite(PUMP3_PIN, LOW);
  pump3Running = false;
  Blynk.virtualWrite(V3, 0);
  Serial.println("Pump 3 STOPPED");
}

void stopAllPumps() {
  stopPump1();
  stopPump2();
  stopPump3();
}

// -------------------------
// Read Sensors + Send to Blynk
// -------------------------
void readAndSendSensors() {
  // Read soil sensors
  soil1 = readSoilPercent(SOIL1_PIN, SOIL1_DRY, SOIL1_WET);
  soil2 = readSoilPercent(SOIL2_PIN, SOIL2_DRY, SOIL2_WET);
  soil3 = readSoilPercent(SOIL3_PIN, SOIL3_DRY, SOIL3_WET);

  // Read temperature sensors
  temp1 = readTemperatureC(TEMP1_PIN);
  temp2 = readTemperatureC(TEMP2_PIN);
  temp3 = readTemperatureC(TEMP3_PIN);

  // Send sensor values to Blynk
  Blynk.virtualWrite(V10, soil1);
  Blynk.virtualWrite(V11, soil2);
  Blynk.virtualWrite(V12, soil3);

  Blynk.virtualWrite(V13, temp1);
  Blynk.virtualWrite(V14, temp2);
  Blynk.virtualWrite(V15, temp3);

  // Debug to Serial Monitor
  Serial.print("Soil1: "); Serial.print(soil1); Serial.print("%  ");
  Serial.print("Soil2: "); Serial.print(soil2); Serial.print("%  ");
  Serial.print("Soil3: "); Serial.print(soil3); Serial.print("%  ");

  Serial.print("Temp1: "); Serial.print(temp1); Serial.print("C  ");
  Serial.print("Temp2: "); Serial.print(temp2); Serial.print("C  ");
  Serial.print("Temp3: "); Serial.print(temp3); Serial.println("C");
}

// -------------------------
// Auto Watering Logic
// -------------------------
void autoWateringCheck() {
  if (!autoMode || emergencyStop) return;

  // If soil is below threshold and pump isn't already running, start pump
  if (soil1 < threshold1 && !pump1Running) {
    startPump1();
  }

  if (soil2 < threshold2 && !pump2Running) {
    startPump2();
  }

  if (soil3 < threshold3 && !pump3Running) {
    startPump3();
  }
}

// -------------------------
// Stop pumps when duration expires
// -------------------------
void managePumpTimers() {
  unsigned long currentMillis = millis();

  if (pump1Running && (currentMillis - pump1StartTime >= (unsigned long)pumpDuration)) {
    stopPump1();
  }

  if (pump2Running && (currentMillis - pump2StartTime >= (unsigned long)pumpDuration)) {
    stopPump2();
  }

  if (pump3Running && (currentMillis - pump3StartTime >= (unsigned long)pumpDuration)) {
    stopPump3();
  }
}

// -------------------------
// Update status LEDs
// -------------------------
void updateStatusLEDs() {
  if (Blynk.connected()) {
    digitalWrite(LED_ONLINE, HIGH);
    digitalWrite(LED_OFFLINE, LOW);
  } else {
    digitalWrite(LED_ONLINE, LOW);
    digitalWrite(LED_OFFLINE, HIGH);
  }

  digitalWrite(LED_AUTO, autoMode ? HIGH : LOW);
}

// -------------------------
// Blynk Connection Event
// -------------------------
BLYNK_CONNECTED() {
  Serial.println("Blynk Connected!");

  // Sync app values after reconnect
  Blynk.syncVirtual(V0, V1, V2, V3, V4, V5, V6, V7, V8);
}

// -------------------------
// Blynk Virtual Pin Handlers
// -------------------------

// V0 = Auto Mode switch
BLYNK_WRITE(V0) {
  autoMode = param.asInt();

  if (autoMode) {
    Serial.println("Auto Mode ENABLED");
  } else {
    Serial.println("Auto Mode DISABLED");
  }
}

// V1 = Manual Pump 1 switch
BLYNK_WRITE(V1) {
  int value = param.asInt();

  // If emergency stop is active, ignore
  if (emergencyStop) {
    stopPump1();
    return;
  }

  if (value == 1) {
    startPump1();
  } else {
    stopPump1();
  }
}

// V2 = Manual Pump 2 switch
BLYNK_WRITE(V2) {
  int value = param.asInt();

  if (emergencyStop) {
    stopPump2();
    return;
  }

  if (value == 1) {
    startPump2();
  } else {
    stopPump2();
  }
}

// V3 = Manual Pump 3 switch
BLYNK_WRITE(V3) {
  int value = param.asInt();

  if (emergencyStop) {
    stopPump3();
    return;
  }

  if (value == 1) {
    startPump3();
  } else {
    stopPump3();
  }
}

// V4 = Threshold 1 slider
BLYNK_WRITE(V4) {
  threshold1 = param.asInt();
  Serial.print("Threshold1 set to: ");
  Serial.println(threshold1);
}

// V5 = Threshold 2 slider
BLYNK_WRITE(V5) {
  threshold2 = param.asInt();
  Serial.print("Threshold2 set to: ");
  Serial.println(threshold2);
}

// V6 = Threshold 3 slider
BLYNK_WRITE(V6) {
  threshold3 = param.asInt();
  Serial.print("Threshold3 set to: ");
  Serial.println(threshold3);
}

// V7 = Pump duration slider (seconds in app, convert to ms)
BLYNK_WRITE(V7) {
  int seconds = param.asInt();
  pumpDuration = seconds * 1000;

  Serial.print("Pump duration set to: ");
  Serial.print(seconds);
  Serial.println(" seconds");
}

// V8 = Emergency Stop switch/button
BLYNK_WRITE(V8) {
  emergencyStop = param.asInt();

  if (emergencyStop) {
    Serial.println("EMERGENCY STOP ACTIVATED!");
    stopAllPumps();
  } else {
    Serial.println("Emergency stop released");
  }
}

// -------------------------
// Setup
// -------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Pump outputs
  pinMode(PUMP1_PIN, OUTPUT);
  pinMode(PUMP2_PIN, OUTPUT);
  pinMode(PUMP3_PIN, OUTPUT);

  // Ensure pumps are OFF at startup
  digitalWrite(PUMP1_PIN, LOW);
  digitalWrite(PUMP2_PIN, LOW);
  digitalWrite(PUMP3_PIN, LOW);

  // Status LEDs
  pinMode(LED_OFFLINE, OUTPUT);
  pinMode(LED_ONLINE, OUTPUT);
  pinMode(LED_AUTO, OUTPUT);

  digitalWrite(LED_OFFLINE, HIGH);
  digitalWrite(LED_ONLINE, LOW);
  digitalWrite(LED_AUTO, LOW);

  Serial.println("Starting Plant Watering Kit...");

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Timed tasks
  timer.setInterval(2000L, readAndSendSensors);   // every 2 sec
  timer.setInterval(3000L, autoWateringCheck);    // every 3 sec
  timer.setInterval(500L, updateStatusLEDs);      // every 0.5 sec

  Serial.println("Setup complete.");
}

// -------------------------
// Main Loop
// -------------------------
void loop() {
  Blynk.run();
  timer.run();

  // Always check if any pump should stop
  managePumpTimers();
}