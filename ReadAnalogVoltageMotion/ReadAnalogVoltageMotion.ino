/*
  ReadAnalogVoltage
  Reads an analog input on pin 0, converts it to voltage, and prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

 This example code is in the public domain.
 */

// the setup routine runs once when you press reset:
int count=0;
int lowMD = 0;
int midMD = 0;
int hiMD = 0;
int topMD = 0;
int circuitbreach = 0;
int MDBreach = 0;
float voltLow = 0;
float voltMid = 0;
float voltHi = 0;
float voltTop = 0;
float voltCBreach = 0;
float voltMDBreach = 0;
boolean low = false;
boolean mid = false;
boolean hi = false;
boolean top = false;
int lowCount = 0;
int midCount = 0;
int hiCount = 0;
int topCount = 0;
boolean alerts[4];

void setup() {
  // initialize serial communication at 9600 bits per second:
 pinMode(13, OUTPUT);
 pinMode(12, OUTPUT);
 pinMode(11, OUTPUT);
 pinMode(10, OUTPUT);
 Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  count++;
  digitalWrite(13, LOW);  // Added this line
  digitalWrite(12, LOW); 
  digitalWrite(11, LOW); 
  // read the input on analog pin 0:
  lowMD = analogRead(A0);
  midMD = analogRead(A1);
  hiMD = analogRead(A2);
  topMD = analogRead(A3);
  circuitbreach = digitalRead(1);
  MDBreach = digitalRead(2);
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  voltLow = lowMD * (5.0 / 1023.0);
  voltMid = midMD * (5.0 / 1023.0);
  voltHi = hiMD * (5.0 / 1023.0);
  voltTop = topMD * (5.0 / 1023.0);
  voltCBreach = circuitbreach * (5.0 / 1023.0);
  voltMDBreach = MDBreach * (5.0 / 1023.0);
  
  if(voltLow>1.0 && voltLow<=2){
    lowCount++;
    Serial.println(lowCount);
    if(lowCount>2){ process(13);
    alerts[0] = true;
    low = true;
     if(lowCount>7){
      lowCount = 0; 
     }}}

  if(voltMid>1.0 && voltMid<=2){
    midCount++;
    Serial.println(midCount);
    if(midCount>2){ process(12);
    alerts[1] = true;
    mid = true;
     if(midCount>7){
      midCount = 0; 
     }}}

  if(voltHi>1.0 && voltHi<=2){
    hiCount++;
    Serial.println(hiCount);
    if(hiCount>2){ process(11);
    alerts[2] = true;
    hi = true;
     if(hiCount>7){
      hiCount = 0; 
     }}}

  if(voltTop>1.0 && voltTop<=2){
    topCount++;
    Serial.println(topCount);
    if(topCount>2){ process(11);
    alerts[3] = true;
    top = true;
     if(topCount>7){
      topCount = 0; 
     }}}

    for(int x = 2; x<4; x++){
    boolean Alm = alerts[x];
    switch(Alm){
      case true:
       int y = 0;
       for(int x = 0; x<4; x++){
       if(alerts[x] == true){
        y++;
      }}
      break;
    }
    }
  delay(1000);
}

void process(int Alarm){
   digitalWrite(Alarm, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(1000);              // wait for a second
     digitalWrite(Alarm, LOW);    // turn the LED off by making the voltage LOW
     delay(1000);      
     digitalWrite(Alarm, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(1000);              // wait for a second
     digitalWrite(Alarm, LOW);    // turn the LED off by making the voltage LOW
     delay(1000); 
}

