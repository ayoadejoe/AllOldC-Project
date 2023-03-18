/*
 * To reprogram esp, you pick the arduino board and connect it as an ftdi
 * To convert board to ftdi
 * 1. Take reset to ground
 * 2. connect esp tx, the top 4th pin to the other side of the red led - to the tx slot of the uno - direct
 * 3. connect the rx pin, lower 1st pin on the led side - to the rx slot of uno via a voltage div network of 2k to gnd and 1k to incoming from uno rx
 * 4. Use 5V supply if you have catered for the step down and 3.3V if you have not
 * 
 * Setting up the ESP
 * 1. connect gpio1 to ground via a press switch
 * 2. connect reset to gnd via a press switch
 * 3. with a 10k, connect rst to vcc
 * 4. connect ch_pd to vcc
 * 
 * To upload your program
 * 1. press reset for 1 sec
 * 2. Then press gpio2 switch.  The blue light will stop blinking and only the red light will glow
 */
 
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#define commPin 13

int values[4];
boolean statusConn = false;
String arduinoData ="No Data yet";

WiFiServer server(8080);      // Create an instance of the server and specify the port to listen on as an argument

IPAddress staticIP(192, 168, 8, 105); 
IPAddress gateway(192, 168, 8, 1); 
IPAddress subnet(255, 255, 255, 0); 

const char* ssid     = "REPENT";
const char* password = "psalm121";

void setup() {
  Serial.begin(115200);
  EEPROM.begin(1024);
  
  WiFi.config(staticIP, gateway, subnet);

  WiFi.begin(ssid, password);
  int to=0;
  while (WiFi.status() != WL_CONNECTED) {
    reAddress();
    delay(500);
    Serial.print(".");
    if(to>20){
      to=0;
      Serial.println("connecting.");
    }
    to++;
  }

  server.begin();
  Serial.println();
  Serial.print("Connecting to Access Point:");
  Serial.println(WiFi.SSID());
  Serial.println("Connected on:");
  Serial.println(WiFi.localIP());
  statusConn = true;
}

void loop() {
  if(statusConn){
  delay(1000);
  digitalWrite(13, LOW);
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String req = client.readString();
  delay(5);
  Serial.println(req);
  client.flush();
 //(req);
 
  while(Serial.available()>0){
      arduinoData = Serial.readString();
      if(arduinoData.substring(0,5).equals("newAd"))writeNewAddress(arduinoData);     //then its a new address
      digitalWrite(13, HIGH);
      delay(10);
      client.print(arduinoData);
      client.flush();
      Serial.flush();
      delay(1);
  }

  if(!Serial.available()>0){
    client.print("No information yet!");
      client.flush();
      Serial.flush();
      delay(1);
  }
  arduinoData ="No Data yet";
  }else{
   Serial.println("status is false");
  }
}

char* getSSID(){
  int q=5; int a=0; byte c;
  while(q<25){
    c = EEPROM.read(q);
    q++; a++;
    if(c==35)break;
    }
  char ssid[a-1];
  for(int w = 0; w<(a-1); w++){
  c = EEPROM.read(w+5);
  ssid[w] = (char)c;
  }
  return ssid;
}

char* getPassword(){
  int q=25, a=0; byte c;
  while(q<45){
    c = EEPROM.read(q);
    q++; a++;
    if(c==35)break;
  }
  char password[a-1];
  for(int w = 0; w<(a-1); w++){
  c = EEPROM.read(w+25);
  password[w] = (char)c;
  }
  return password;
}

int aa=0;
char* IPMemory(){
  int q=45; byte c;
  String e = "";
  while(q<65){
    c = EEPROM.read(q);
    q++; aa++;
    if(c==35)break;
  }
}

int ww=0;

int oct(int part){
  String e = "";
  int p = 0;
  byte c;
 while( ww<(aa-1)){
    c = EEPROM.read(ww+45);
    if(c==46) {ww++;break;}
    String y = String((char)c);
    e.concat(y);
    p  = e.toInt();
    ww++;
  }
  if(part == 4) ww = 0;
  return p;
}

void reAddress(){
              if(Serial.available()>0){
                String message = Serial.readString();
                if(message.substring(0,5).equals("newAd"))writeNewAddress(message);
              }
}


void  writeNewAddress(String message){
  int e = 0;

  String sid = message.substring(5, 25);
  String pw = message.substring(25, 45);
  String ip = message.substring(45, 65);
  
  char it;
  byte d;
  while (e<message.length()){
    it = message.charAt(e);
    d = (byte)it;
    EEPROM.write(e, d);
    e++;
  }
  if(e==message.length()){
    EEPROM.commit();
    delay(1000);
  }
}
