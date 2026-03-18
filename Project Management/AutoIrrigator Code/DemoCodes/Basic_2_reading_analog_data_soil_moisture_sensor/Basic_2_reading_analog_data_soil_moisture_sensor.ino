#define HUMI3 A0
#define HUMI2 A1
#define HUMI1 A2


void setup() {
   Serial.begin(9600);

}

void loop() {
   int HUMI1_Raw_data = analogRead(HUMI1);
   int HUMI2_Raw_data = analogRead(HUMI2);
   int HUMI3_Raw_data = analogRead(HUMI3);

   Serial.print("Soil moisture sensor raw data: ");
   Serial.print("CH1: ");
   Serial.print(HUMI1_Raw_data);
   Serial.print(" CH2: ");
   Serial.print(HUMI2_Raw_data);
   Serial.print(" CH3: ");
   Serial.println(HUMI3_Raw_data);
   delay(1000);
}
