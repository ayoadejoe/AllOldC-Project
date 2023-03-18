#define solenoid100 A1
#define solenoid75 A2
#define pump A3

void setup() {
 Serial.begin(9600);
 pinMode(solenoid100, OUTPUT);
 pinMode(solenoid75, OUTPUT);
 pinMode(pump, OUTPUT);
}

void loop() {
   digitalWrite(13, LOW);
                digitalWrite(solenoid100, HIGH);
                digitalWrite(13, HIGH);
                Serial.println("Solenoid100 ON");
                delay(7000);
                digitalWrite(solenoid100, LOW);
                Serial.println("Solenoid100 OFF");
                digitalWrite(pump, HIGH);
                Serial.println("pump on");
                digitalWrite(13, LOW);
                delay(7000);
                digitalWrite(pump, LOW);
                Serial.println("pump OFF");
               digitalWrite(solenoid75, HIGH);
              digitalWrite(13, HIGH);
            Serial.println("Solenoid75 HIGH");
            delay(5000);
            digitalWrite(solenoid75, LOW);
            Serial.println("Solenoid75 LOW");
}
