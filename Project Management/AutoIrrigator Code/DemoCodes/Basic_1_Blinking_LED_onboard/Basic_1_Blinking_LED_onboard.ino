#define Green_LED  6
#define Red_LED 7

void setup() {
    pinMode(Green_LED, OUTPUT);
    pinMode(Red_LED, OUTPUT);
}

void loop(){
    digitalWrite(Green_LED, HIGH);
    digitalWrite(Red_LED, HIGH);
    delay(500);
    digitalWrite(Green_LED, LOW);
    digitalWrite(Red_LED, LOW);
    delay(500);
}