// DS1302_LCD 
// Copyright (C)2016 Ayoade Adetunji. All right reserved
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
// web: http://www.litconcept.net
//
// A quick demo of how to use my DS1302-library to make a quick
// clock using a DS1302 and a 16x2 LCD.
//
// I assume you know how to connect the DS1302 and LCD.
// DS1302:  CE pin    -> Arduino Digital 2
//          I/O pin   -> Arduino Digital 3
//          SCLK pin  -> Arduino Digital 4
// LCD:     DB7       -> Arduino Digital 6
//          DB6       -> Arduino Digital 7
//          DB5       -> Arduino Digital 8
//          DB4       -> Arduino Digital 9
//          E         -> Arduino Digital 10
//          RS        -> Arduino Digital 11

#include <LiquidCrystal.h>
#include <DS1302.h>
#include <math.h>
// Init the DS1302
DS1302 rtc(2, 3, 4);

// Init the LCD
LiquidCrystal lcd(11, 10, 9, 8, 7, 6);

// make some custom characters:
byte TankBase[8] = {
  0b11111,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b11111
};

String words= "";
void setup()
{
  Serial.begin(9600);
  Serial.println("Starting");
  pinMode(A5, INPUT);
  pinMode(A4, INPUT);
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);
  
  // Setup LCD to 16x2 characters
  lcd.begin(16, 2);

  // create a new character
  lcd.createChar(1, TankBase);
}

void loop()
{
 while(Serial.available())  {
     words = Serial.readString();
     Serial.println(words);
     lcd.clear();
     processWords(words);
     lcd.clear();
  }

  analogResistance();
  analogResistance2();
 
  //Joseph Line
 /* 
  Display time centered on the upper line
  lcd.setCursor(0, 0);
  lcd.print(rtc.getTimeStr());
  
  lcd.setCursor(10, 0);
  // draw the tank base, arms down:
  lcd.write(1);

  lcd.setCursor(0, 1);
  lcd.print("JOE");

  lcd.setCursor(3, 1);
  // draw the little man, arms down:
  lcd.write(3);
  
  // Display date in the lower right corner
  lcd.setCursor(5, 1);
  lcd.print(rtc.getDateStr());
  
  delay (500);
 // draw him arms up:
  lcd.setCursor(3, 1);
  lcd.write(4);
  // Wait one second before repeating :)
  */
  delay (1000);
}


void processWords(String string){
  int lent = string.length();
  lcd.setCursor(0,0);
  lcd.print(string.substring(0,16));
  
  if(lent>16){
    lcd.setCursor(0,1);
    lcd.print(string.substring(16,lent));
  }
  delay(5000);
}

void analogResistance(){
  float outV = analogRead(A5);
  float resistance = (5.0*outV)-3940;
  float remaining = computeVolume(resistance);
  lcd.setCursor(0,1);
  lcd.print("Tank 100> ");
  lcd.setCursor(10,1);
  lcd.print(remaining);
  lcd.setCursor(15,1);
  lcd.print("L");
}

float computeVolume(float lev){
  float vol = (lev/1135.0)*200;
  return vol;
}

void analogResistance2(){
  float outV = analogRead(A4);
  float resistance = (5.0*outV)-3650;
  
  Serial.print(resistance);
  Serial.println(" Ohms.");
  
  float remaining = computeVolume2(resistance);
  Serial.print((remaining));
  Serial.println(" Litres.");
  lcd.setCursor(0,0);
  lcd.print("Tank 75> ");
  lcd.setCursor(10,0);
  lcd.print(remaining);
  lcd.setCursor(15,0);
  lcd.print("L");
}

float computeVolume2(float lev){
  float vol = (lev/1170.0)*200;
  return vol;
}

