
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include <IRremoteESP8266.h>
#include <ESP8266WiFi.h>
    
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

IRrecv irrecv(D3);
decode_results results;
//#define SS_PIN 4
//#define RST_PIN 2
MFRC522 mfrc522(D4, D2); 
String IDNo;
byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM
boolean successRead = false;
boolean successRead2 = false;

WiFiServer server(8080);

const char* ssid = "REPENT";
const char* password = "psalm121";

unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
const long interval = 5000; 
unsigned long currentMillis = 0;

IPAddress staticIP(192, 168, 8, 102); 
IPAddress gateway(192, 168, 8, 1); 
IPAddress subnet(255, 255, 255, 0); 

WiFiClient client;

boolean conn = false;
///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {
  Serial.begin(115200);  // Initialize serial communications with PC
  delay(10);

  currentMillis = millis();
  //Protocol Configuration
  pinMode(D1, OUTPUT);
  pinMode(D0, OUTPUT);
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
   Serial.println(F("Access Control Example v0.1"));   // For debugging purposes
    ShowReaderDetails();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println("Scan PICC to see UID and type...");
  digitalWrite(D1, LOW);
  irrecv.enableIRIn(); // Start the receiver

  digitalWrite(D0, HIGH);
  Serial.println("Door Open");
  delay(5000);
  digitalWrite(D0, LOW);
  Serial.println("Door Close");

  WiFi.config(staticIP, gateway, subnet);

  WiFi.begin(ssid, password);
}

boolean connector(){
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  conn = true;
}

void loop() {
  if(!conn) conn = connector();
  if(conn){
      client = server.available();

      while(client.available()){
      String req = client.readString();
      delay(5);
      Serial.print("*:");
      Serial.println(req);
      client.flush();
      delay(3);
      client.print("Success!");
      client.flush();
      delay(2);
      }
  }
  
  while(!client.available()){
  delay(1);
  while(Serial.available()>0){
    String input = Serial.readString();
    
     while(input.equals("newID")){
      Serial.println("Follow the prompts:");
      input = getNewID();
      digitalWrite(D1, HIGH);
      delay(1000);
      digitalWrite(D1, LOW);
     }
  }


  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if(!successRead2){
      successRead2 = true;
    }else{
      successRead2 = false;
    }

    if(successRead2){
       digitalWrite(D0, HIGH);
       Serial.println("Door Open");
    }else{
      digitalWrite(D0, LOW);
      Serial.println("Door Open");
    }
    irrecv.resume(); // Receive the next value
  }
  
 if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())return;

  // Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  // Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX) + (i!=3 ? ":" : "");
  digitalWrite(D1, HIGH);
  }
  strID.toUpperCase();
  Serial.println(F("Scanned PICC's UID:"));
  Serial.print("Tap card key: ");
  Serial.println(strID);
  
  if(strID.equals("25:50:20:77")){
    if(successRead){
      successRead=false;
    }else{
    successRead = true;
    }
  }

  if(successRead){
    digitalWrite(D0, HIGH);
    Serial.println("Door Open");
  }else{
    digitalWrite(D0, LOW);
    Serial.println("Door Closed");
  }
  mfrc522.PICC_HaltA(); // Stop reading
  mfrc522.PCD_StopCrypto1();
  digitalWrite(D1, LOW);
  
  }
}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown),probably a chinese clone?"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    Serial.println(F("SYSTEM HALTED: Check connections."));
    while (true); // do not go further
  }
}


String IDwhite, IDBlue;
String getNewID(){
  Serial.println("Please Enter Blue Card ID>");
      while (Serial.available()<1){
        Serial.println("Please Enter Blue Card ID>");
        delay(5000);
      }
      if(Serial.available()>0){
        IDBlue = Serial.readString();
      }

      if(IDBlue.equals("X"))return "error";             //error handler

      Serial.println("Please Enter White Card ID>");
      while (Serial.available()<1){
        Serial.println("Please Enter ID White>");
        delay(5000);
      }
      if(Serial.available()>0){
        IDwhite = Serial.readString();
      }

      if(IDwhite.equals("X"))return "error";             //error handler
      
      return "2done";
}



void  writeNewAddress(String blue, String white){
  int e = 0;
  int f = 0;
  
  char it;
  byte d;

  while (e<blue.length()){
    it = blue.charAt(e);
    d = (byte)it;
    EEPROM.write(e, d);
    e++;
  }

  while(f<white.length()){
    it = white.charAt(f);
    d = (byte)it;
    EEPROM.write((e+f), d);
    f++;
  }
  
  if(f==white.length()){
    EEPROM.commit();
    delay(1000);
  }
}

/*
 *String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX) + (i!=3 ? ":" : "");
  }
  strID.toUpperCase();
  Serial.print("Tap card key: ");
  Serial.println(strID);
 * /
 */


