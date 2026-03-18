#include <IRremote.h>     // IR remote header file
#include <Arduino.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>  // include the library

#define water_pump_1 2 
#define water_pump_2 3 
#define water_pump_3 4 

#define IR_RECEIVER_PIN 5
#define DECODE_NEC

#define LED_G  6  // Green_LED
#define LED_R  7  // Red_LED

void setup() {
    Serial.begin(115200);

    pinMode(LED_G, OUTPUT);
    pinMode(LED_R, OUTPUT);

    pinMode(water_pump_1, OUTPUT);
    pinMode(water_pump_2, OUTPUT);
    pinMode(water_pump_3, OUTPUT);

    // enable IR
    IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);

    printActiveIRProtocols(&Serial);
}

int count = 3;
int channel = 0; 
const int flag = 3;  // three channels 

void loop() {
   if (IrReceiver.decode()) {
    /*
      * print a summary of received data
      */
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      Serial.println(F("Received noise or an unknown (or not yet enabled )protocol"));
      // we have an unknown protocol here, print extend info
      IrReceiver.printIRResultRawFormatted(&Serial, true);
      IrReceiver.resume();
      // Do it here to preserve raw data for printing with printIRResultRawFormatted()
    } else {
      IrReceiver.resume();
      IrReceiver.printIRResultShort(&Serial);
      IrReceiver.printIRSendUsage(&Serial);
    }
    Serial.println();
    Serial.println(IrReceiver.decodedIRData.command);

    if (IrReceiver.decodedIRData.command == 0x40 && count % flag == 0) {
      // press on button on IR controller will generate this data 0x40
      digitalWrite(LED_G, HIGH); // turn on green led 
      count+=1;  // add to count variable  
      digitalWrite(water_pump_1, HIGH); // turn on the water pump 1.
      IrReceiver.resume(); // resume IR 
      digitalWrite(LED_G, LOW);

    } else if (IrReceiver.decodedIRData.command == 0x40 && count % flag == 1) {
      digitalWrite(LED_G, HIGH); // turn on green led 
      count+=1;  // add to count variable  
      digitalWrite(water_pump_2, HIGH); // turn on the water pump 2.
      IrReceiver.resume(); // resume IR 
       digitalWrite(LED_G, LOW);
    } else if (IrReceiver.decodedIRData.command == 0x40 && count % flag == 2) {
      digitalWrite(LED_G, HIGH); // turn on green led 
      count+=1;  // add to count variable  
      digitalWrite(water_pump_3, HIGH); // turn on the water pump 3.
      IrReceiver.resume(); // resume IR
       digitalWrite(LED_G, LOW); 
    } else if (IrReceiver.decodedIRData.command == 0x19 && count % flag == 1) {
      digitalWrite(LED_R, HIGH); // turn off green led 
      count+=1;  // add to count variable  
      digitalWrite(water_pump_1, LOW); // turn on the water pump 1.
      IrReceiver.resume();
      digitalWrite(LED_R, LOW); // turn on red led 

    } else if (IrReceiver.decodedIRData.command == 0x19 && count % flag == 2) {
      digitalWrite(LED_R, HIGH); // turn off green led 
      count+=1;  // add to count variable  
      digitalWrite(water_pump_2, LOW); // turn on the water pump 1.
      IrReceiver.resume();
      digitalWrite(LED_R, LOW); // turn on red led 

    } else if (IrReceiver.decodedIRData.command == 0x19 && count % flag == 0) {
      digitalWrite(LED_R, HIGH); // turn off green led 
      count+=1;  // add to count variable  
      digitalWrite(water_pump_3, LOW); // turn on the water pump 1.
      IrReceiver.resume();
      digitalWrite(LED_R, LOW); // turn on red led 
    }  
   }
}

