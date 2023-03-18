#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

//GASOLINE GENERATOR REMOTE & IOT PROJECT

//Rf sensing input pins
uint8_t PinValue0 =0;
uint8_t PinValue1 =0;
uint8_t PinValue2 =0;

uint8_t updateCounter =0;

//Pin declarations
static const uint8_t Pin0   = 16;   //ON
static const uint8_t Pin1   = 5;    //OFF
static const uint8_t Pin2   = 4;    //Network mode
//3 & 4 can not be used
static const uint8_t Pin3   = 0;
static const uint8_t Pin4   = 2;

static const uint8_t Pin5   = 14;   //STOP
static const uint8_t Pin6   = 12;   //START
static const uint8_t Pin7   = 13;  //THROTTLE
static const uint8_t Pin8   = 15;     //ALARM/NETWORK
static const uint8_t PinA0 = 17;  //checks if Gen is on ADC

double genVolt = 0.0;
int sleepTime = 0;
boolean sleepActive = false;
boolean genNowOn= false;
boolean alarmActive = false;
String wifloco="";

boolean genOnMarker=false;
boolean sleepMarker=false;
boolean alarmMarker=false;
//D3 on logic high flashes the mcu, D4 on high makes it impossible to upload - both keys cannot be used

//NETWORKING SECTION
#ifndef STASSID
#define STASSID "fruitful"
#define STAPSK  "AwesomeGod"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* deviceName = "Mahogani";

const char* autobotSSID = "AUTOBOT";
const char* autobotPassword = "Psalm121";

const char* josephSSID = "IQJOY";
const char* josephPassword = "jojo1234";

const char* host = "www.i-margin.com";

const char* extIPHelpHost = "www.myexternalip.com";    // Get external IP from https://myexternalip.com/raw

String publicIP = "";

ESP8266WebServer server(8073);

IPAddress staticIP(192, 168, 43, 190); //ESP static ip
IPAddress gateway(192, 168, 43, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8,8,8,8);

IPAddress autobotIP(192, 168, 8, 120); //ESP static ip
IPAddress autobotGateway(192, 168, 8, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress autobotSubnet(255, 255, 255, 0);  //Subnet mask


//HANDLES SECTION
void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected to Mahogani_V1.0</h1><h2>Commands: /ON, /OFF, /SLEEP?sleep=mins, /STOPSLEEP, /STATUS </h2><a href='/'>Home</a></body></html>");
}

void handleSTATUS() {
  String oN = "nothing";
  String sleep = "SleepInactive";
  String sleeptim = "0";
  if(genNowOn)oN = "TRUE";
    else oN="FALSE";
    
  if(sleepActive){
    sleep = "TRUE";
    sleeptim =+ sleepTime;
  }
  
  server.send(200, "text/html", "<h1>Mahogani_V1.0 Status: ON> "+oN+", Sleep> "+sleep+", Sleep Time Remaining> "+sleeptim+" seconds</h1><a href='/'>Home</a></body></html>");
}

void handleON() {                          // If a POST request is made to URI /LED
    
  if(!genNowOn){
    if(startGen())server.send(200, "text/html", "<html><body><h1>Gen is now ON</h1><a href='/'>Home</a></body></html>");
      else server.send(200, "text/html", "<html><body><h1>Gen is not switching ON</h1><a href='/'>Home</a></body></html>");
  }
  else server.send(200, "text/html", "<html><body><h1>Gen is either already ON</h1><a href='/'>Home</a></body></html>");
}


void handleOFF() {                          // If a POST request is made to URI /LED
  if(genNowOn){
  if(stopGen())server.send(200, "text/html", "<html><body><h1>Gen is now OFF</h1><a href='/'>Home</a></body></html>");
    else server.send(200, "text/html", "<html><body><h1>Gen is not switching OFF</h1><a href='/'>Home</a></body></html>");
  }
  else server.send(200, "text/html", "<html><body><h1>Gen is already OFF</h1><a href='/'>Home</a></body></html>");
}

void handleSTOPSLEEP() {                          // If a POST request is made to URI /LED
  if(sleepActive){
    sleepActive = false;
    sleepTime = 0;
    server.send(200, "text/html", "<html><body><h1>Sleep Deactivated</h1><a href='/'>Home</a></body></html>");
  }
  else server.send(200, "text/html", "<html><body><h1>Sleep is already inactive</h1><a href='/'>Home</a></body></html>");
}

void handleSLEEP(){
  Serial.print("Sleep function accessed - Argument no:");
 Serial.println(server.args());

  if (server.args() > 0 ) { // Arguments were received

    for ( uint8_t i = 0; i < server.args(); i++ ) {
      String argument_name = server.argName(i);
      Serial.print(server.argName(i)); // Display the argument
      if (server.argName(i) == "sleep") {
        Serial.print(" Input received was: ");
        Serial.println(server.arg(i));
        sleepTime = server.arg(i).toInt();
        sleepTime = sleepTime*60;
        sleepActive = true;
        /* e.g. range_maximum = server.arg(i).toInt();   // use string.toInt()   if you wanted to convert the input to an integer number
          Clients_Response2 = server.arg(i);
          e.g. range_maximum = server.arg(i).toFloat(); // use string.toFloat() if you wanted to convert the input to a floating point number
        */
      }
   }
   String sslp ="";
   sslp=+ sleepTime;
  server.send(200, "text/html", "<html><body><h1>Gen will sleep for "+sslp+" seconds.</h1><a href='/'>Home</a></body></html>");
  }else server.send(200, "text/html", "<html><body><h1>No Sleep range sent!</h1><a href='/'>Home</a></body></html>");

}




void setup() {
  pinMode(Pin0, INPUT);
  pinMode(Pin1, INPUT);
  pinMode(Pin2, INPUT);
  
  pinMode(Pin5, OUTPUT);
  pinMode(Pin6, OUTPUT);
  pinMode(Pin7, OUTPUT);
  pinMode(Pin8, OUTPUT);

  pinMode(PinA0, INPUT);
  digitalWrite(Pin7, HIGH);
  digitalWrite(Pin6, HIGH);
  digitalWrite(Pin5, HIGH);
  digitalWrite(Pin8, HIGH);
  
  delay(1000);
  Serial.begin(115200);
  Serial.println();

int numberOfNetworks = WiFi.scanNetworks();
 
  for(int i =0; i<numberOfNetworks; i++){
      
      Serial.print("Network name: ");
      Serial.println(WiFi.SSID(i));
      Serial.print("Signal strength: ");
      Serial.println(WiFi.RSSI(i));
      Serial.println("-----------------------");

      //if autobot is available
      if(WiFi.SSID(i).equals("AUTOBOT")){
        Serial.println("Connecting to AUTOBOT");
        WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
        WiFi.config(autobotIP, dns, autobotGateway, subnet); 
        WiFi.mode(WIFI_STA);
        WiFi.begin(autobotSSID, autobotPassword);

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        wifloco = "192.168.8.120";
        break;
      }

//if my phone is available
      if(WiFi.SSID(i).equals("IQJOY")){
        Serial.println("Connecting to Joseph");
        WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
        WiFi.config(staticIP, dns, gateway, subnet);
        WiFi.mode(WIFI_STA);
        WiFi.begin(josephSSID, josephPassword);

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        wifloco = "192.168.43.190";
        break;
      }

      //if franca's phone is available
      if(WiFi.SSID(i).equals("fruitful")){
        Serial.println("Connecting to fruitful");
        WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
        WiFi.config(staticIP, dns, gateway, subnet);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        wifloco = "192.168.43.190";
        break;
      }
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/ON", handleON);
  server.on("/OFF", handleOFF);
  server.on("/SLEEP", handleSLEEP);
  server.on("/STATUS", handleSTATUS);
  server.on("/STOPSLEEP", handleSTOPSLEEP);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS successful");
    Serial.println("MDNS responder started");
  }
  server.begin();
  Serial.println("HTTP server started");
}


void loop() {
  updateCounter = updateCounter+1;
  server.handleClient();
  MDNS.update();
  Serial.print("Connected on>");
  wifloco = WiFi.localIP().toString();
  Serial.println(wifloco);

  int found = wifloco.indexOf("unset");
  if(found>0){
    Serial.print("Something wrong with wifi, IP is not registered: Restarting in ...");
    Serial.println(updateCounter);
    if(updateCounter == 20){
      Serial.println("Lost WiFi Connection! Restarting...");
      ESP.restart();
    }
    
    if(alarmActive){
      Serial.println("Alarm activated");
      alarm();
    }
  }else{
    if(updateCounter == 3 || updateCounter == 50 || updateCounter == 900){
    digitalWrite(Pin8, HIGH);
    delay(1500);
    digitalWrite(Pin8, LOW);}
  }

  //ON GEN LOGIC
  PinValue0 = digitalRead(Pin0);
  Serial.print("Pin 0 is ");
  Serial.println(PinValue0);
  if(PinValue0 == 1){
   if(!genNowOn)startGen();
    else Serial.println("Gen is already ON");
  }else{
    digitalWrite(Pin7, HIGH);
    digitalWrite(Pin6, HIGH);
  }

  //OFF GEN LOGIC
  PinValue1 = digitalRead(Pin1);
  Serial.print("Pin 1 is ");
  Serial.println(PinValue1);
  if(PinValue1 == 1){
    if(genNowOn)stopGen();
      else Serial.println("Gen is already OFF");
  }else(digitalWrite(Pin5, HIGH));

  //SWITCH ALARM ACTIVE
  PinValue2 = digitalRead(Pin2);
  Serial.print("Pin 2 is ");
  Serial.println(PinValue2);
  if(PinValue2 == 1){
    if(!alarmActive){
      alarmActive = true;
      Serial.println("Alarm is armed");
      digitalWrite(Pin8, HIGH);
      delay(500);
      digitalWrite(Pin8, LOW);
      delay(300);
      digitalWrite(Pin8, HIGH);
      delay(500);
      digitalWrite(Pin8, LOW);
    }else{
      alarmActive = false;
      Serial.println("Alarm is unarmed");
      digitalWrite(Pin8, HIGH);
      delay(500);
      digitalWrite(Pin8, LOW);
      delay(300);
      digitalWrite(Pin8, HIGH);
      delay(500);
      digitalWrite(Pin8, LOW);
    }
  }


  //SLEEP STATUS
  if(sleepActive){
    sleepTime = sleepTime-1;
    Serial.print("Sleep time remaining: ");
    Serial.print(sleepTime);
    Serial.println( " secs");
    if(sleepTime<1){
      stopGen();
      Serial.println("Shutting Down: Sleep completed");
      sleepActive = false;
    }
  }
  //FEEDBACK: keep checking if Generator remains ON
  genStatus();

//to update online when gen goes on and off
  if(!genOnMarker && genNowOn){
    updateCounter = 0;
    genOnMarker = true;
  }

  if(genOnMarker && !genNowOn){
    updateCounter = 0;
    genOnMarker = false;
  }

  //to update online when sleep goes true or false
  if(!sleepMarker && sleepActive){
    updateCounter = 0;
    sleepMarker = true;
  }

  if(sleepMarker && !sleepActive){
    updateCounter = 0;
    sleepMarker = false;
  }

  //to update online when alarm active/inactive
  if(!alarmMarker && alarmActive){
    updateCounter = 0;
    alarmMarker = true;
  }

  if(alarmMarker && !alarmActive){
    updateCounter = 0;
    alarmMarker = false;
  }

//Regular online registration of status
  if(updateCounter == 3 || updateCounter == 50 || updateCounter == 900){
    if(updateCounter == 900) updateCounter =0;
    HTTPClient http, http2;    //Declare object of class HTTPClient

  //first get public ip
    http.begin("http://api.ipify.org");  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
     
    if (httpCode > 0) { //Check the returning code
    publicIP = http.getString();   //Get the request response payload
    Serial.println(publicIP);                     //Print the response payload
    }
    http.end();   //Close connection

 //send to server ( for record
  String oN = "nothing";
  String sleep = "SleepInactive";
  String sleeptim = "0";
  String alam = "nothing";
  if(genNowOn)oN = "TRUE";
    else oN="FALSE";

  if(alarmActive)alam="TRUE";
    else alam = "FALSE";
    
  if(sleepActive){
    sleep = "TRUE";
    sleeptim =+ sleepTime;
  }

  String Link = "http://i-margin.com/GeneratorStatus.php/?IP=";
    Link=Link+publicIP; 
    Link=Link+"&State="+oN;
    Link=Link+"&Sleep="+sleep; Link=Link+"&SleepTime="+sleeptim;
    String wifssid = WiFi.SSID(); 
    Link=Link+"&Wifi="+wifssid; 
    Link=Link+"&WifiLocalIP="+wifloco;
    Link=Link+"&AlarmStatus="+alam;
    Serial.println(Link);
    http2.begin(Link);  //Specify request destination
    int http2Code = http2.GET();                                                                  //Send the request
     
    if (http2Code > 0) { //Check the returning code
    String payload = http2.getString();   //Get the request response payload
    Serial.print("response:");
    Serial.println(payload);                     //Print the response payload
    }
     
    http2.end();   //Close connection
  }

   if(WiFi.status() == WL_DISCONNECTED){
      Serial.println("Lost WiFi Connection! Restarting...");
      ESP.restart();
    }

    delay(1000);
}

//GEN FUNCTIONS
boolean startGen(){

  //initiate throttle
    digitalWrite(Pin7, LOW);
    Serial.println("Throttle Pin 7 now active");
    delay(4000);
    //initiate starter
    digitalWrite(Pin6, LOW);
    Serial.println("Starter Pin 6 now active");
    delay(5000);
    digitalWrite(Pin6, HIGH);
    Serial.println("disengaged starter");
    delay(2000);
//Now, check if gen is now ON - we expect it will be unstable because throttle is still engaged
    if(genStatus()){
      //disengage throttle to finalise ON
      Serial.println("disengaging throttle");
      digitalWrite(Pin7, HIGH);
      genNowOn = true;  // although declared true already in genStatus, still declare true just in case
      return true;
    }else{
      //if gen did not come ON
      delay(2000);
      Serial.println("Gen did not come ON - press ON again if you'd like to try again");
      genNowOn = false;  // although declared false already in genStatus, still declare false just in case
      return false;
    }
}

boolean stopGen(){
    digitalWrite(Pin5, LOW);
    Serial.println("Ignition pin 5 is now active - shutting down");
    delay(5000);
    digitalWrite(Pin5, HIGH);
    genNowOn = false;
    return true;
}

boolean genStatus(){
  genVolt = analogRead(PinA0);
  Serial.println();
  Serial.print("Voltage=");
  Serial.println(genVolt);
  Serial.println();
  //Alarm
  if(genVolt > 1000){
    Serial.println("Generator is ON");
    Serial.println();
    genNowOn = true;
    return true;
  }else{
    genNowOn = false;
    Serial.println("Generator is OFF");
    Serial.println();
    return false;
  }
}

 void alarm(){
    digitalWrite(Pin8, LOW);
    delay(2500);
    digitalWrite(Pin8, HIGH);
  }
