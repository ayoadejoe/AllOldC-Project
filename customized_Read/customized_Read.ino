
int pink =0;
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
  Serial.begin(115200);
  pinMode(Pin0, INPUT);
  pinMode(Pin1, INPUT);
  pinMode(Pin2, INPUT);
  pinMode(Pin3, INPUT);
  pinMode(Pin4, INPUT);
  pinMode(Pin5, INPUT);
  pinMode(Pin6, INPUT);
  pinMode(Pin7, INPUT);
  pinMode(Pin8, OUTPUT);
  
  pinMode(PinA0, INPUT);

}

void loop() {
 Serial.println("STARTING");

  pink = digitalRead(Pin0);    // turn the LEPin off by making the voltage LOW
  if(pink == HIGH) analogWrite(Pin8, 20); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  else Serial.println("pin0 is low");
  Serial.print("Pin0 check:");
  Serial.println(pink);
  delay(2000); 
  
  pink = digitalRead(Pin1);    // turn the LEPin off by making the voltage LOW
  Serial.print("Pin1 check:");
  Serial.println(pink);
  if(pink == HIGH) analogWrite(Pin8, 255); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  else Serial.println("pin1 is low");
  delay(2000); 
  
  pink = digitalRead(Pin2);    
  Serial.print("Pin2 check:");
  Serial.println(pink);
  if(pink == HIGH) analogWrite(Pin8, 40); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  else Serial.println("pin2 is low");
  delay(2000);

  pink = digitalRead(Pin3);   // turn the LEPin on (check: is the voltage level)
  Serial.print("pin3 Check:");
  Serial.println(pink);
  if(pink == HIGH) analogWrite(Pin8, 240); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  else Serial.println("pin3 is low");
  delay(2000); 

  pink = digitalRead(Pin4);    // turn the LEPin off by making the voltage LOW
  Serial.print("Pin4 check:");
  Serial.println(pink);
  if(pink == HIGH) analogWrite(Pin8, 60); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  else Serial.println("pin4 is low");
  delay(2000); 

  pink = digitalRead(Pin5);    
  Serial.print("Pin5 check:");
  Serial.println(pink);
  if(pink == HIGH) analogWrite(Pin8, 220);
  else Serial.println("pin5 is low"); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  delay(2000);

  pink = digitalRead(Pin6);   // turn the LEPin on (check: is the voltage level)
 Serial.print("Pin6 check:");
  Serial.println(pink);
  if(pink == HIGH) analogWrite(Pin8, 80); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  else Serial.println("pin6 is low");
  delay(2000); 

  pink = digitalRead(Pin7);    
  Serial.print("Pin7 check:");
  Serial.println(pink);
  if(pink == HIGH) analogWrite(Pin8, 200); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  else Serial.println("pin7 is low");
  delay(2000);

}
