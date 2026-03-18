#include <TFT_eSPI.h>
#include <TFT_eWidget.h>


#define moisture_1 A2
#define moisture_2 A1

// create instance of TFT display
TFT_eSPI tft = TFT_eSPI();

// define meters rh1 - moisture_1 , rh2 - moisture_2
MeterWidget rh1 = MeterWidget(&tft);
MeterWidget rh2 = MeterWidget(&tft);

void setup() {
  Serial.begin(115200);

  // init tft display and clear screen
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_WHITE);

  // setting meters parameters for soil moistures
  rh1.setZones(0, 25, 25, 50, 50, 75, 75, 100);
  rh1.analogMeter(0, 0, 100.0, "CH1", "DRY", "DRY", "MID", "WET", "WET");
  
  rh2.setZones(0, 25, 25, 50, 50, 75, 75, 100);
  rh2.analogMeter(0, 120, 100.0, "CH2", "DRY", "DRY", "MID", "WET", "WET");
}

void loop() {
  float value_1 = analogRead(moisture_1);
  float value_2 = analogRead(moisture_2);

  rh1.updateNeedle((100 - value_1 / 10.24), 0);
  rh2.updateNeedle((100 - value_2 / 10.24), 0);

  delay(20);
}
