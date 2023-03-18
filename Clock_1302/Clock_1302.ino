// DS1302_Serial_Easy 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my DS1302-library to 
// quickly send time and date information over a serial link
//
// I assume you know how to connect the DS1302.
// DS1302:  CE pin    -> Arduino Digital 2
//          I/O pin   -> Arduino Digital 3
//          SCLK pin  -> Arduino Digital 4

#include <DS1302.h>

// Init the DS1302
DS1302 rtc(9,10, 11);  
Time t;
void setup()
{
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);
  
  // Setup Serial connection
  Serial.begin(9600);
  t = rtc.getTime();
  // The following lines can be commented out to use the values already stored in the DS1302
  //rtc.setDOW(WEDNESDAY);        // Set Day-of-Week to FRIDAY
  //rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(6, 8, 2010);   // Set the date to August 6th, 2010
}

void loop()
{
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.println(" ");

  Serial.print("seconds:");
  Serial.print(t.sec);
  int secn = t.sec;
  Serial.println(" -- ");

  Serial.print("minutes:");
  Serial.print(t.min);
  Serial.println(" -- ");
  
  Serial.print("hour:");
  Serial.print(t.hour);
  Serial.println(" -- ");

  Serial.print("day:");
  Serial.print(t.date);
  Serial.println(" -- ");
  
  Serial.print("month:");
  Serial.print(t.mon);
  Serial.println(" -- ");
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());
  
  // Wait one second before repeating :)
  delay (10000);
}
