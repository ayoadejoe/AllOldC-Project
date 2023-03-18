#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices

    #define D0 16
    #define D1 5
    #define D2 4
    #define D3 0
    #define D4 2
    #define D5 14
    #define D6 12
    #define D7 13
    #define D8 15
    #define D9 3
    #define D10 1


//#define SS_PIN 4
//#define RST_PIN 2
MFRC522 mfrc522(D4, D2); 
byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM
boolean successRead = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Starting");
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
}


void loop() {
  digitalWrite(D0, HIGH);
  Serial.println("Working");
  delay(2000);
  digitalWrite(D0, LOW);
  delay(1000);
  bingo();
  delay(500);
}


void bingo(){
  digitalWrite(D1, HIGH);
  delay(300);
  digitalWrite(D1, LOW);
  delay(300);
  digitalWrite(D1, HIGH);
  delay(300);
  digitalWrite(D1, LOW);
  delay(300);
}


