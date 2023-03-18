
/*
For SDCard
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 * 
 * For Time:DS1302:  
 *  CE pin    -> Arduino Digital 2
 *  I/O pin   -> Arduino Digital 5
 *  SCLK pin  -> Arduino Digital 4
 * 

*/

#include <DS1302.h>
#include <SPI.h>
#include <SD.h>

static const uint8_t Pin0   = 16; 
static const uint8_t Pin1   = 5; 
static const uint8_t Pin2   = 4; 
static const uint8_t Pin3   = 0; 
static const uint8_t Pin4   = 2; 
static const uint8_t Pin5   = 14; 
static const uint8_t Pin6   = 12; 
static const uint8_t Pin7   = 13; 
static const uint8_t Pin8   = 15; 
static const uint8_t Pin9   = 3; 
static const uint8_t Pin10  = 1;

//Init the DS1302
DS1302 rtc(2, 5, 4);
const int chipSelect = 4;



void setup() {
 Serial.begin(115200);
 Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  Serial.println("Starting Clock");
  //Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);

  //The following lines can be commented out to use the values already stored in the DS1302
  rtc.setDOW(MONDAY);        // Set Day-of-Week to FRIDAY
  rtc.setTime(14, 30, 0);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(3, 4, 2017);   // Set the date to August 6th, 2010
  

}

int y=0;
void loop() {
  y=y+1;
  String dataString = String(y)+": I AM WORKING WELL";
  String g1 = String(rtc.getTimeStr());
  String g2 = String(rtc.getDOWStr(FORMAT_SHORT));
  String g3 = String(rtc.getDateStr());
  String g0 = String(y);
  dataString = String(g0+" = "+g1+"]"+g2+"]"+g3);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
    delay(2000);
    
    if(y=10){
      SD.remove("datalog.txt");
      Serial.println("Deleting file");
      return;
    }
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    delay(2000);
  }

}
