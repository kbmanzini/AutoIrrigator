#include <TFT_eSPI.h>     // Hardware-specific library
#include <TFT_eWidget.h>  // Widget library
#include <Math.h>         // import Math library
#include <vector>         // import vector library
#include <IRremote.h>     // IR remote header file
#include <Arduino.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>  // include the library
// #include <SoftwareSerial.h>
// #include <string.h>

// SoftwareSerial mySerial(0,1);  // initial software serial  instance.

// define IR pin default on Pin 5 - D5
#define IR_RECEIVE_PIN 5   
#define DECODE_NEC

// define pump's singal pin
#define relay_1 2  // Relay connected to D2, motor 1
#define relay_2 3  // Relay connected to D3, motor 2
#define relay_3 4  // Relay connected to D4, motor 3

//define temperature sensor pin
#define temp_1 A5  // NTC No.1
#define temp_2 A4  // NTC No.2
#define temp_3 A3  // NTC No.3

// define soil moisture's singal pins
#define moisture_1 A2  //soil moisture No.1
#define moisture_2 A1  //soil moisture No.2
#define moisture_3 A0  //soil moisture No.3

// define LED indicators onboard
#define LED_1 6     // Green LED 
#define LED_2 7     // Red LED

// setting a structure to store tempvalue and index information
struct TempValue {
  int value;
  size_t index;
};

// here stored the temp_table of the temperatue -> resistor table and it has been convert to ADC reading range from 0-1023.
// check the table of NTC 10K resister table and convert it to adc reading range by following formula
//  resister number / (resister number + 10000.0)
// for example:  when temperatrue is 25 degree, the NTC's resistor number is 10000.0 ohm, so the adc number will be:
//  10000.0 / (10000.0 + 10000.0) = 512, so, if you reading from adc and get 512 +/- 10 equals 25 degree.
// dut to the NTC dose not a linear component, so you need to check the table to grab the temperature value.
std::vector<int> temp_table = {
  994, 992, 990, 988, 986, 983, 981, 978, 975, 972, 969, 966, 962, 959, 955, 951, 947, 943, 938, 933,
  928, 923, 918, 912, 907, 901, 894, 888, 881, 874, 867, 860, 852, 845, 837, 828, 820, 811, 802, 793,
  784, 774, 765, 755, 745, 735, 724, 714, 703, 692, 682, 671, 659, 648, 637, 626, 614, 603, 592, 580,
  569, 557, 546, 535, 523, 512, 501, 490, 479, 468, 457, 446, 436, 425, 415, 405, 395, 385, 375, 365,
  356, 347, 338, 329, 320, 311, 303, 295, 287, 279, 271, 264, 256, 249, 242, 235, 229, 222, 216, 210,
  204, 198, 193, 187, 182, 176, 172, 167, 162, 157, 153, 148, 144, 140, 136, 132, 128, 125, 121, 118,
  114, 111, 108, 105, 102, 99, 96, 94, 91, 88, 86, 84, 81, 79, 77, 75, 73, 71, 69, 67, 65, 63, 62, 60,
  58, 57, 55, 54, 52, 51, 50, 48, 47, 46, 45, 44, 43, 41, 40, 39, 38, 37, 36, 36, 35, 34, 33, 32, 31, 31,
  30, 29, 28, 28, 27, 26, 26, 25, 25, 24, 23, 23, 22, 22, 21, 21, 20, 20, 20, 19, 19
};

// customize the abs value
int custom_abs(int x) {
  return (x >= 0) ? x : -x;
}

// function  to check the temp_table by sending adc reading value.
TempValue closestNumber(const std::vector<int>& temp_table, int value) {
  TempValue result;
  result.value = temp_table[0];
  result.index = 0;

  int min_diff = custom_abs(temp_table[0] - value);

  for (size_t i = 1; i < temp_table.size(); ++i) {
    int diff = custom_abs(temp_table[i] - value);
    if (diff < min_diff) {
      min_diff = diff;
      result.value = temp_table[i];
      result.index = i;
    }
  }
  result.index -= 40;
  return result;
}

// create instance of TFT display
TFT_eSPI tft = TFT_eSPI();

// define meters rh1 - soil moisture, rh2 - NTC temperature
MeterWidget rh1 = MeterWidget(&tft);
MeterWidget rh2 = MeterWidget(&tft);


// char buf[100]={0};     // define a buffer for receving the data from serial port.
// #define ACK_ERROR 1
// #define ACK_OK 0

// // define a method to send command to ESP01 via serial port 
// uint8_t Send_Commnd(char* commnd ,uint8_t*ack,  unsigned long time)
// {
//   memset(buf,0,sizeof(buf));
//   while(*commnd)  //一个一个字母发送
//   {
//     mySerial.print(*commnd);
//     commnd++;
//   }
//   mySerial.readBytes(buf,sizeof(buf));
//   Serial.println(buf);  // print to serial port for debugging.

//   delay(time);
 
//   if(strstr((char *)buf,(char *)ack) == NULL) // check if there is `ack` keyword in ESP01S's printing content. 
//   {
//     return ACK_ERROR;   
//   }
//   else
//   {
//     return ACK_OK;
//   }     
// }

// sending data to server 
// // data for sending -> string
// void Send(char *string)
// {
//   uint64_t len; //data length

//   len= strlen(string);

//   char buf[255];
//   sprintf(buf, "AT+CIPSEND=%d\r\n",len);  //combine the data length and AT command to one package. 

//   while(Send_Commnd(buf,(uint8_t*)"OK",1000)!= ACK_OK); //sending AT command to ESP01S 

//   delay(5);

//   mySerial.println(string); //sending data 

// }

// //ESP01S connect to wifi and sending data to server 
// void wifi_init(void)
// {
//   while(Send_Commnd((char*)"AT\r\n",(uint8_t*)"OK",500)!= ACK_OK);  // test AT command 
//   while(Send_Commnd((char*)"AT+RESTORE\r\n",(uint8_t*)"OK",500)!= ACK_OK);   // restore to factory settings 
//   while(Send_Commnd((char*)"AT+CWMODE=1\r\n",(uint8_t*)"OK",1000)!= ACK_OK);  // 1 for station mode , 2 for softAP mode , 3 for station + softAP mode. 
//   while(Send_Commnd((char*)"AT+CWJAP=\"HUAWEI-B4NKSR\",\"stm32f429\"\r\n",(uint8_t*)"OK",1000)!= ACK_OK){  // connect to wifi, SSID and password. 
//     digitalWrite(LED_2, HIGH); //if ACK_OK failure, light up red led.
//   };
//     digitalWrite(LED_2, LOW);   // turn off red led.
//     digitalWrite(LED_1, HIGH);  // turn on green led. means wifi connection is ok. 
//   while(Send_Commnd((char*)"AT+CIFSR\r\n",(uint8_t*)"OK",(uint8_t)500)!= ACK_OK);  // sending AT command to get IP address and MAC information. 
//   // connect to server 192.168.3.100 via TCP protocol.
//   //while(Send_Commnd((char*)"AT+CIPSTART=\"TCP\",\"192.168.3.100\",777\r\n",(uint8_t*)"CONNECT",1000)!= ACK_OK);
//   // Setting CIPMODE=0 to set common transmission mode, in this mode ,you need to spicify the data length.
//   // while(Send_Commnd((char*)"AT+CIPMODE=0\r\n",(uint8_t*)"OK",(uint8_t)1000)!= ACK_OK);
//   // CIPSNED=4 will send 4 byte data 
//   //while(Send_Commnd((char*)"AT+CIPSEND\r\n",(uint8_t*)"OK",(uint8_t)1000)!= ACK_OK);
//   //while(Send_Commnd((char*)"AT+CIPSEND=4\r\n",(uint8_t*)"OK",500)!= ACK_OK);
//   //mySerial.print("1234");
//   // Send("123.123445f");
//   // Serial.println("wifi init ok\r\n");
// }



void setup() {
  Serial.begin(115200);
  // mySerial.begin(115200);

  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  pinMode(relay_3, OUTPUT);
  // enable IR
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  printActiveIRProtocols(&Serial);

  // init tft display and clear screen
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_WHITE);

  // setting meters parameters for soil moisture
  rh1.setZones(0, 25, 25, 50, 50, 75, 75, 100);
  rh1.analogMeter(0, 0, 100.0, "CH1", "DRY", "DRY", "MID", "WET", "WET");

  // setting meters parameters for NTC tempearatures
  rh2.setZones(0, 100, 20, 80, 30, 85, 40, 70);
  rh2.analogMeter(0, 120, 35.0, "CH1", "0", "15", "25", "30", "35");

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);

  // wifi_init();
}


int count = 1;
const int flag = 3;
int channel = 0; 

void loop() {

  float value_1 = analogRead(moisture_1);
  float value_2 = analogRead(moisture_2);
  float value_3 = analogRead(moisture_3);

  int sensorvalue1 = analogRead(temp_1);
  int sensorvalue2 = analogRead(temp_2);
  int sensorvalue3 = analogRead(temp_3);

  // delay(20);
  // digitalWrite(LED_1, HIGH);
  // digitalWrite(LED_2, HIGH);
  // delay(20);
  // digitalWrite(LED_1, LOW);
  // digitalWrite(LED_2, LOW);

  TempValue temp_1 = closestNumber(temp_table, sensorvalue1);
  TempValue temp_2 = closestNumber(temp_table, sensorvalue2);
  TempValue temp_3 = closestNumber(temp_table, sensorvalue3);

  switch (channel) {
    case 1:
      rh1.updateNeedle((100 - value_1 / 10.24), 0);
      rh2.updateNeedle(temp_1.index, 0);
      break;
    case 2:
      rh1.updateNeedle((100 - value_2 / 10.24), 0);
      rh2.updateNeedle(temp_2.index, 0);
      break;
    case 3:
      rh1.updateNeedle((100 - value_3 / 10.24), 0);
      rh2.updateNeedle(temp_3.index, 0);
      break;
    default:
      rh1.updateNeedle((100 - value_1 / 10.24), 0);
      rh2.updateNeedle(temp_1.index, 0);
      break;
  }

 delay(500);  // delay in between reads for stability

  if (IrReceiver.decode()) {

    /*
         * Print a summary of received data
         */
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      // Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
      // We have an unknown protocol here, print extended info
      IrReceiver.printIRResultRawFormatted(&Serial, true);
      IrReceiver.resume();  // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
    } else {
      IrReceiver.resume();  // Early enable receiving of the next IR frame
      IrReceiver.printIRResultShort(&Serial);
      IrReceiver.printIRSendUsage(&Serial);
    }
    // Serial.println();
    // Serial.println(IrReceiver.decodedIRData.command);

    if (IrReceiver.decodedIRData.command == 0x40 && count % flag == 1) {
      digitalWrite(relay_1, HIGH);
      rh1.analogMeter(0, 0, 100.0, "CH1", "DRY", "DRY", "MID", "WET", "WET");
      rh2.analogMeter(0, 120, 35.0, "CH1", "0", "15", "25", "30", "35");
      count += 1;
      channel = 1;
    } else if (IrReceiver.decodedIRData.command == 0x40 && count % flag == 2) {
      digitalWrite(relay_2, HIGH);
      rh1.analogMeter(0, 0, 100.0, "CH2", "DRY", "DRY", "MID", "WET", "WET");
      rh2.analogMeter(0, 120, 35.0, "CH2", "0", "15", "25", "30", "35");
      count += 1;
      channel = 2;
    } else if (IrReceiver.decodedIRData.command == 0x40 && count % flag == 0) {
      digitalWrite(relay_3, HIGH);
      rh1.analogMeter(0, 0, 100.0, "CH3", "DRY", "DRY", "MID", "WET", "WET");
      rh2.analogMeter(0, 120, 35.0, "CH3", "0", "15", "25", "30", "35");
      count += 1;
      channel = 3;
    } else if (IrReceiver.decodedIRData.command == 0x19 && count % flag == 1) {
      digitalWrite(relay_1, LOW);
      rh1.analogMeter(0, 0, 100.0, "CH1", "DRY", "DRY", "MID", "WET", "WET");
      rh2.analogMeter(0, 120, 35.0, "CH1", "0", "15", "25", "30", "35");
      count += 1;
      channel = 1;
    } else if (IrReceiver.decodedIRData.command == 0x19 && count % flag == 2) {
      digitalWrite(relay_2, LOW);
      rh1.analogMeter(0, 0, 100.0, "CH2", "DRY", "DRY", "MID", "WET", "WET");
      rh2.analogMeter(0, 120, 35.0, "CH2", "0", "15", "25", "30", "35");
      count += 1;
      channel = 2;
    } else if (IrReceiver.decodedIRData.command == 0x19 && count % flag == 0) {
      digitalWrite(relay_3, LOW);
      rh1.analogMeter(0, 0, 100.0, "CH3", "DRY", "DRY", "MID", "WET", "WET");
      rh2.analogMeter(0, 120, 35.0, "CH3", "0", "15", "25", "30", "35");
      count += 1;
      channel = 3;
    }
    if (count > 3){
      count = 1;
    }
  }
}
