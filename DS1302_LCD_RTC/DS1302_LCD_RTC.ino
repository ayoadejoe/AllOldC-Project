// DS1302_LCD 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
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

// Init the DS1302
DS1302 rtc(2, 3, 4);

// Init the LCD
LiquidCrystal lcd(11, 10, 9, 8, 7, 6);

// make some custom characters:
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte smiley[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

byte frownie[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001
};

byte armsDown[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01010
};

byte armsUp[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00100,
  0b01010
};

String words = "";

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting");
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);
  
  // Setup LCD to 16x2 characters
  lcd.begin(16, 2);

  // create a new character
  lcd.createChar(0, heart);
  // create a new character
  lcd.createChar(1, smiley);
  // create a new character
  lcd.createChar(2, frownie);
  // create a new character
  lcd.createChar(3, armsDown);
  // create a new character
  lcd.createChar(4, armsUp);

  // Print a message to the lcd.
  lcd.print("I "); 
  lcd.write(byte(0)); // when calling lcd.write() '0' must be cast as a byte
  lcd.print(" Arduino! ");
  lcd.write((byte) 1);
  // The following lines can be commented out to use the values already stored in the DS1302
  //rtc.setDOW(FRIDAY);        // Set Day-of-Week to FRIDAY
  //rtc.setTime(17, 38, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(28, 8, 2016);   // Set the date to August 6th, 2010
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
  // Display time centered on the upper line
  lcd.setCursor(0, 0);
  lcd.print(rtc.getTimeStr());
  
  // Display abbreviated Day-of-Week in the lower left corner
  lcd.setCursor(9, 0);
  lcd.print(rtc.getDOWStr(FORMAT_SHORT));

  //Joseph Line
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
  delay (1000);

  loverman();
}

int linePosition = 0;
int cursorPosition = 0;
  
void loverman(){
  linePosition = 0;
  cursorPosition = 0;
  lcd.clear();
  delay(500);
  String me = "I love francisca, I love riverjoy";
  lcd.setCursor(cursorPosition, linePosition);
  lcd.print(me);
  
  if(me.length()>16){
    nextLine();
  }
  
  lcd.setCursor(cursorPosition, linePosition);
  lcd.print(me.substring(17, me.length()));
  
  delay(2500);
  lcd.clear();

}

void nextLine(){
  linePosition = 1;
  cursorPosition = 0;
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


