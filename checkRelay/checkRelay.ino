#define relaypump 2
#define relaysol1 4
#define relaysol2 3
#define rel 13
#define echo100 5
#define trig100 6
#define echo75 7
#define trig75 8
#define ON100 1
#define INDICATE100 A3
#define INDICATE75 A2
#define ON75 0

#define blueTrig 10
#define blueEcho 9
#define grayTrig 12
#define grayEcho 11

#define alert 13

#include "pitches.h"
// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int state = 0;
int ty = 0;
int outV = 0;
int op100 = 0, op75 = 0;
int resistance = 0;
byte incomingByte;
boolean GEN100ON = false, save100 = true, save75 = true;
boolean GEN75ON = false;
int accessBlue;
int accessGray;
int totalTank;
long timer100 = 0, oldtime100 = 0, t100;
long timer75 = 0, oldtime75 = 0, t75;
long s = 0; long b = 0;
boolean first = true;
boolean secnd = true;

#include <Time.h>
#include <EEPROM.h>

void setup() {
 Serial.begin(9600);
 pinMode(relaypump, OUTPUT);
 pinMode(relaysol1, OUTPUT);
 pinMode(relaysol2, OUTPUT);

 pinMode(echo100, INPUT);
 pinMode(trig100, OUTPUT);

 pinMode(echo75, INPUT);
 pinMode(trig75, OUTPUT);
 
 pinMode(INDICATE100, OUTPUT);
 pinMode(INDICATE75, OUTPUT);

 pinMode(alert, OUTPUT);

 pinMode(ON100, INPUT);
 pinMode(ON75, INPUT);

  pinMode(blueTrig, OUTPUT);
  pinMode(blueEcho, INPUT);
  pinMode(grayTrig, OUTPUT);
  pinMode(grayEcho, INPUT);

  timer75 = getOldtime(0);
  timer100 = getOldtime(4);
  
  Serial.println("Beep! -Automation started");

  Serial.print("Inicial Time 100:");
  Serial.println(timer100);

  Serial.print("Inicial Time 75:");
  Serial.println(timer75);
}

int TimeCheck100(int hr, int mint, int sec){
    int hh = 0; int mm = 0; int ss= 0, t100 = 0;
    if(hr >0){
      hh = hr*3600;
    }

    if(mint>0){
      mm = mint*60;
    }
    t100 = hh+mm+sec;
    Serial.print("Time100:");
    Serial.println(t100);
    Serial.println();
    return t100;
}

int TimeCheck75(int hr, int mint, int sec){
    int hh = 0; int mm = 0; int ss= 0, t75 = 0;
    if(hr >0){
      hh = hr*3600;
    }

    if(mint>0){
      mm = mint*60;
    }
    t75 = hh+mm+sec;
  Serial.print("Time75:");
  Serial.println(t75);
  Serial.println();
  return t75;
}

void MainCheck(){
   if(GEN100ON){
    t100 = TimeCheck100(hour(), minute(), second());
    if(op100>10){
    updateTime(4, (timer100+t100));
   Serial.print("Timer100 Update:");
   Serial.println((timer100+t100));
    op100=5;
    }
    save100 = true;
  }else{
    if(save100){    //gen was just switched off, save data
    Serial.print("Latest Time 100:");
    Serial.println((timer100+t100));
    updateTime(4, (timer100+t100));
    EEPROM.update(50, accessBlue);
    EEPROM.update(51, accessGray);
    EEPROM.update(52, totalTank);
    save100 = false;
    t100 = 0;
    }
  }
  
  if(GEN75ON){
    t75 = TimeCheck75(hour(), minute(), second());
    if(op75 >10){
    updateTime(0, (timer75+t75));
    Serial.print("Timer75:");
    Serial.println((timer75));
    Serial.print("Timer75+t75 Update:");
    Serial.println((timer75+t75));

    Serial.print("Timer75:");
    Serial.println((timer75));
    op75=5;
    }
    save75 = true;
  }else{
    if(save75){
    Serial.print("Latest Time 75:");
    Serial.println((timer75+t75));
    updateTime(0, (timer75+t75));
    EEPROM.update(50, accessBlue);
    EEPROM.update(51, accessGray);
    EEPROM.update(52, totalTank);
    save75 = false;
    t75 = 0;
  }
}
}

long getOldtime(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }

      
//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void updateTime(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.update(address, four);
      EEPROM.update(address + 1, three);
      EEPROM.update(address + 2, two);
      EEPROM.update(address + 3, one);
      }


void checkReset(){
  if(GEN100ON== false && first == true){
     setTime(0, 0, 0, 0, 0, 0);
      Serial.print("100KVA just switched off- Reseting time");
      Serial.println();
    // setTime(hr,min,sec,day,month,yr); reset Time
    first = false;
  }

  if(GEN75ON == false && secnd == true){
     setTime(0, 0, 0, 0, 0, 0);
     Serial.print("75KVA just switched off- Reseting time: secnd=");
     Serial.println(secnd);
     Serial.println();
    // setTime(hr,min,sec,day,month,yr); 
    secnd = false;
  }
}

void loop() {
  digitalWrite(13, LOW);
  op100++;
  op75++;
  delay(3000);
  
  GEN100ON = checkON(ON100, INDICATE100);
  GEN75ON = checkON(ON75, INDICATE75);
  
   if(GEN100ON) {
    first = true;
     Serial.print("STATUS100 ON:");
     Serial.println(timer100+t100);
   }else{
     Serial.print("STATUS100 OFF:");
     Serial.println(timer100+t100);
   }

    delay(3000);
    
   if(GEN75ON) {
    secnd = true;
    Serial.print("STATUS75 ON:");
    Serial.println(timer75+t75);
   }else{
     Serial.print("STATUS75 OFF:");
     Serial.println(timer75+t75);
   }
   
  checkReset();

  if(GEN100ON == true && GEN75ON == true){
  }else{
    if(GEN75ON == false && GEN100ON == false){
    }else{
    MainCheck();
    }
  }
  
  accessBlue = analyse(blueTrig, blueEcho);
  Serial.print("LEVELBLUE:");
  Serial.println(accessBlue);

  accessGray = analyse(grayTrig, grayEcho);
  Serial.print("LEVELGRAY:");
  Serial.println(accessGray);
  totalTank = accessBlue+accessGray;
  
  if(GEN100ON == false){
    digitalWrite(INDICATE100, LOW);
    int access = analyse(trig100, echo100);
    Serial.print("LEVEL100:");
    Serial.println(access);
    if(access>4 && access<=30 && op100>2){
    if(totalTank < 320){
    digitalWrite(INDICATE100, LOW);
    soundAlert2(3);
    int rel1 = digitalRead(relaysol1);
    if(rel1 == LOW){
    digitalWrite(relaysol1, HIGH);
    }
    delay (5000);
    
    rel1 = digitalRead(relaysol1);
    if(rel1 == HIGH){
          //update before refill
      Serial.print("Timer75 Update:");
       Serial.println((timer75+t75));
       updateTime(0, (timer75+t75));
    
       Serial.print("Timer100 Update:");
       Serial.println((timer100+t100));
       updateTime(4, (timer100+t100));
       
       while(access > 4){
       Serial.println("100LOW");
       Serial.println("REFILL100");
       int pum = digitalRead(relaypump);
       if(pum == LOW){
       digitalWrite(relaypump, HIGH);
       }
       delay (1000);
       access = analyseRefill(trig100, echo100);
       Serial.print("100 Present Level:");
       Serial.println(access);
       soundAlert2(1);
       }  

       digitalWrite(relaypump, LOW);
       delay (5000);
       digitalWrite(relaysol1, LOW);
      }
    }else{
       Serial.print("Tank Fuel Level is Low:");
       Serial.println(totalTank);
    }
  }else{
    digitalWrite(relaypump, LOW);
    delay (5000);
    digitalWrite(relaysol1, LOW);
    //Serial.println("Generator 100KVA has fuel");
  }  
  }else{
    MainCheck();
  }
  
  GEN100ON = checkON(ON100, INDICATE100);
  GEN75ON = checkON(ON75, INDICATE75);
  
  if(GEN75ON == false){
    digitalWrite(INDICATE75, LOW);
    int access2 = analyse(trig75, echo75);
    Serial.print("LEVEL75:");
    Serial.println(access2);
    if(access2> 4 && access2 <= 20 && op75 > 2){
    if(totalTank<320){
    digitalWrite(INDICATE75, LOW);
    soundAlert(3);
    int rel2 = digitalRead(relaysol2);
    if(rel2 == LOW){
    digitalWrite(relaysol2, HIGH);
    Serial.println("This rel2 loop remained accessed");
    }
    delay (5000);
    Serial.println(access2);
    digitalWrite(relaysol2, HIGH);
    rel2 = digitalRead(relaysol2);
    Serial.println("Rel2 is ");
    Serial.println(rel2);
    if(rel2 == HIGH){
      //update before refill
      Serial.print("Timer75 Update:");
       Serial.println((timer75+t75));
       updateTime(0, (timer75+t75));
    
       Serial.print("Timer100 Update:");
       Serial.println((timer100+t100));
       updateTime(4, (timer100+t100));
       
       while(access2 > 4){
       Serial.println("75LOW");
       Serial.println("REFILL75");
       int pum = digitalRead(relaypump);
       if(pum == LOW){
       digitalWrite(relaypump, HIGH);
       }
       access2 = analyseRefill(trig75, echo75);
       Serial.print("75 Present Level:");
       Serial.println(access2);
       delay (1000);
       
       soundAlert(1);
       }  

    digitalWrite(relaypump, LOW);
    delay (3000);
    
    digitalWrite(relaysol2, LOW);
      }
    }else{
       Serial.print("Tank Fuel Level is Low:");
       Serial.println(totalTank);
    }
  }else{
    digitalWrite(relaypump, LOW);
    delay (3000);
    
    digitalWrite(relaysol2, LOW);
  }
   
  }else{
    MainCheck();
  }
  
  GEN100ON = checkON(ON100, INDICATE100);
  GEN75ON = checkON(ON75, INDICATE75);
  delay(3000);
  Serial.println("ROLL");
  if(totalTank > 310){
      Serial.print("Tank Fuel Level is Low:");
      Serial.println(totalTank);
  }
  delay(2000);
/*
 if(op>13){
   Serial.print("Timer75 Update:");
   Serial.println((timer75+t75));
   updateTime(0, (timer75+t75));

   Serial.print("Timer100 Update:");
   Serial.println((timer100+t100));
   updateTime(4, (timer100+t100));
   op = 5;
 }*/
}

 int analyse(int trigPin, int echoPin){
  long duration, distance;  
  int cum = 0; int ave = 0;
   
  for(int count = 0; count<10; count++){
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 2.91;
  cum+=(distance/10);
  if(count == 9){
    ave = cum/10;
    return ave;
  }
  
  delay(3000);
  }
}

boolean checkON(int genpin, int theGen){
  outV = analogRead(genpin);
  resistance = 5.0*outV;
  resistance = resistance/1000;
  Serial.print("Resistance:");
   Serial.println((resistance));
  if(resistance >= 2){
    if(digitalRead(theGen)==LOW){
    digitalWrite(theGen, HIGH);
    }
    return true;
  }else{
    if(digitalRead(theGen)==HIGH){
    digitalWrite(theGen, LOW);
    }
    return false;
  }
}

 void soundAlert(int no){
  while(no>0){
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(alert, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(alert);
  }
  no--;
  }
 }

 void soundAlert2(int no){
  while(no>0){
  for (int thisNote = 8; thisNote > 0; thisNote--) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(alert, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(alert);
  }
  no--;
  }
 }

 
 int analyseRefill(int trigPin, int echoPin){
  long duration, distance;  
  int cum = 0; int ave = 0;
  for(int count = 0; count<10; count++){
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  //delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 2.91;
  cum+=(distance/10);
  if(count == 2){
    ave = cum/3;
    return ave;
  }
  
  delay(3000);
  } 
}
