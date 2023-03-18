
int Pin0 = 16;
int Pin1 = 5;

int Pin2 = 4;
int Pin3 = 0;

int Pin4 = 2;
int Pin5 = 14;

int Pin6 = 12;
int Pin7 = 13;

int Pin8 = 15;
int PinA0 = 17;

void setup() {
  // initialize digital pin 13 as an output.
  Serial.begin(115200);
  pinMode(Pin0, OUTPUT);
  pinMode(Pin1, OUTPUT);
  pinMode(Pin2, OUTPUT);
  pinMode(Pin3, OUTPUT);
  pinMode(Pin4, OUTPUT);
  pinMode(Pin5, OUTPUT);
  pinMode(Pin6, OUTPUT);
  pinMode(Pin7, OUTPUT);
  pinMode(Pin8, OUTPUT);
  
  pinMode(PinA0, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("STARTING");

  digitalWrite(Pin0, HIGH);    // turn the LEPin off by making the voltage LOW
  Serial.println("Pin0 High");
  delay(2000); 
  
  digitalWrite(Pin1, HIGH);    // turn the LEPin off by making the voltage LOW
  Serial.println("Pin1 High");
  delay(2000); 

  digitalWrite(Pin2, HIGH);    
  Serial.println("Pin2 High");
  delay(2000);

  digitalWrite(Pin3, HIGH);   // turn the LEPin on (HIGH is the voltage level)
  Serial.println("Three ON");
  delay(2000); 

  digitalWrite(Pin4, HIGH);    // turn the LEPin off by making the voltage LOW
  Serial.println("Pin4 High");
  delay(2000); 

  digitalWrite(Pin5, HIGH);    
  Serial.println("Pin5 High");
  delay(2000);

  digitalWrite(Pin6, HIGH);   // turn the LEPin on (HIGH is the voltage level)
  Serial.println("SIX ON");
  delay(2000); 

  digitalWrite(Pin7, HIGH);    
  Serial.println("Pin7 High");
  delay(2000);

  digitalWrite(Pin8, HIGH);   // turn the LEPin on (HIGH is the voltage level)
  Serial.println("PIN8 ON");
  delay(2000); 

  analogWrite(PinA0, 100);   // turn the LEPin on (HIGH is the voltage level)
  delay(2000); 
  analogWrite(PinA0, 50);
  delay(2000); 
  analogWrite(PinA0, 200);
  Serial.println("PINA0 ON");
  delay(2000); 

  digitalWrite(Pin0, LOW);
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW); 
  digitalWrite(Pin4, LOW);
  digitalWrite(Pin5, LOW);
  digitalWrite(Pin6, LOW); 
  digitalWrite(Pin7, LOW);
  digitalWrite(Pin8, LOW); 
  analogWrite(PinA0, 10); 
  Serial.println("ALL OFF");
  delay(2000);       
}
