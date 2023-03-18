#define solenoid100 A1
#define solenoid75 A2
#define pump A3

void setup() {
 Serial.begin(9600);
 pinMode(solenoid100, OUTPUT);
 pinMode(solenoid75, OUTPUT);
 pinMode(pump, OUTPUT);
 pinMode(13, OUTPUT);
}


void loop() {
  digitalWrite(pump, HIGH);//red
  Serial.print("Pump high");
  Serial.println();
  digitalWrite(13, HIGH);
 delay(5000);
 digitalWrite(pump, LOW);
  digitalWrite(solenoid100, HIGH);
  Serial.print("100 high");
    Serial.println();
  digitalWrite(13, LOW);
  delay(5000);
  digitalWrite(solenoid100, LOW);
  digitalWrite(solenoid75, HIGH);
  Serial.print("75 high");
    Serial.println();
  digitalWrite(13, HIGH);
  delay(5000);
  digitalWrite(solenoid75, LOW);
  digitalWrite(13, LOW);
  
}

