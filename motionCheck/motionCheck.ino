int count=0;
int lowMD = 0;
int midMD = 0;
int hiMD = 0;
int lowCount = 0;
float voltLow = 0;
float voltMid = 0;
float voltHi = 0;
boolean alerts[4];
boolean low = false;
boolean mid = false;
boolean hi = false;
int midCount = 0;
int hiCount = 0;
void setup() {
 pinMode(13, OUTPUT);
 pinMode(12, OUTPUT);
 pinMode(11, OUTPUT);
 Serial.begin(9600);

}

void loop() {
  lowMD = analogRead(A0);
  voltLow = lowMD * (5.0 / 1023.0);
  /*Serial.print("Low Coach Strength:" );
  Serial.print(lowMD);
  Serial.print("  <>Low Coach Voltage=: " );*/
  Serial.println(voltLow);
  if(lowCount == 0){
     //Serial.println("  >> No Signal Received Yet");
  }
   if(voltLow>1.1 && voltLow<=2.5){
    lowCount++;
    Serial.println(lowCount);
    if(lowCount>2){ process(13);
    alerts[0] = true;
    low = true;
     if(lowCount>7){
      lowCount = 0; 
     }}}


    midMD = analogRead(A1);
    voltMid = midMD * (5.0 / 1023.0);
    /*Serial.print("Mid Coach Strength:" );
    Serial.print(midMD);
    Serial.print("  <>Mid Coach Voltage=: " );*/
    Serial.println(voltMid);
    if(midCount == 0){
     //Serial.println("  >> No Signal Received Yet");
    }
    if(voltMid>1.1 && voltMid<=2.5){
    midCount++;
    Serial.println(midCount);
    if(midCount>2){ process(12);
    alerts[1] = true;
    mid = true;
     if(midCount>7){
      midCount = 0; 
     }}}

    hiMD = analogRead(A2);
    voltHi = hiMD * (5.0 / 1023.0);
    /*Serial.print("Top Coach Strength:" );
    Serial.print(hiMD);
    Serial.print("  <>Top Coach Voltage=: " );*/
    Serial.println(voltHi);
    if(voltHi>1.1 && voltHi<=2.5){
    hiCount++;
    Serial.println(hiCount);
    if(hiCount>2){ process(11);
    alerts[2] = true;
    hi = true;
     if(hiCount>7){
      hiCount = 0; 
     }}}
    Serial.println("------------");
    
    int y = 0;
    for(int x = 0; x<3; x++){
    boolean Alm = alerts[x];
    switch(Alm){
      case true:
       for(int d = 0; d<3; d++){
       if(alerts[d] == true){
        y++;
      }}
      break;
    }
    if(y>2){
       Serial.println("Multiple Alarm Signal");
       Serial.println("------------");
       y=0;
    }
    }
  delay(3000);
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
