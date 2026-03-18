#include <IRremote.h>  // IR remote header file
#include <Arduino.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>  // include the library

#define IR_RECEIVE_PIN 5
#define DECODE_NEC

// define LED indicators on board
#define LED_1 6
#define LED_2 7


void setup() {
  Serial.begin(115200);

  // enable IR
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  printActiveIRProtocols(&Serial);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
}

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
    // Serial.println();
    // Serial.println(IrReceiver.decodedIRData.command);
    Serial.println(IrReceiver.decodedIRData.decodedRawData);


    if (IrReceiver.decodedIRData.command == 0x40) {
      // press on button on IR controller will generate this data 0x40
      digitalWrite(LED_1, HIGH);
      digitalWrite(LED_2, HIGH);
      IrReceiver.resume();
    } else if (IrReceiver.decodedIRData.command == 0x19) {
      // press off button on IR controller will generate this data 0x19
      digitalWrite(LED_1, LOW);
      digitalWrite(LED_2, LOW);
      IrReceiver.resume();
    }
  }
}
