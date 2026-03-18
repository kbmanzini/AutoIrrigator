#include <Math.h>         // import Math library
#include <vector>         // import vector library
#include <string.h>


#define TEMP3 A3
#define TEMP2 A4
#define TEMP1 A5 

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

void setup() {
   Serial.begin(9600);
}

void loop() {
  
   float ntc_1_raw_data = analogRead(TEMP1);
   float ntc_2_raw_data = analogRead(TEMP2);
   float ntc_3_raw_data = analogRead(TEMP3);

   TempValue ntc_1_temp = closestNumber(temp_table, ntc_1_raw_data);
   TempValue ntc_2_temp = closestNumber(temp_table, ntc_2_raw_data);
   TempValue ntc_3_temp = closestNumber(temp_table, ntc_3_raw_data);

   Serial.println("Temperature of NTC thermistors");
   Serial.print("\tNTC1: ");
   Serial.print(ntc_1_temp.index);
   Serial.print("\tNTC2: ");
   Serial.print(ntc_2_temp.index);
   Serial.print("\tNTC3: ");
   Serial.println(ntc_3_temp.index);

   delay(200); // wait for a while.

}
