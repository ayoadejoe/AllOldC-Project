
#include "soundnotes.h"
int sensor = 0;
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(4, LOW);
  sensor = digitalRead(2);
  if(sensor == HIGH){
     digitalWrite(4, HIGH);
     Serial.println("Sensor detected motion - tone plays");
       for (int thisNote = 0; thisNote < 8; thisNote++) {
         int noteDuration = 1000 / noteDurations[thisNote];
         tone(8, melody[thisNote], noteDuration);
          int pauseBetweenNotes = noteDuration * 1.30;
          delay(pauseBetweenNotes);
          // stop the tone playing:
          noTone(8);
       }
  }else{
     digitalWrite(4, LOW);
     Serial.println("Sensor is off");
  }
 delay(2000);
}
