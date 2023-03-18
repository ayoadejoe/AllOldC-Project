
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


/* Set these to your desired credentials. */
const char *ssid = "MAHOGANI_01";
const char *password = "John10:10";



//GASOLINE GENERATOR REMOTE & IOT PROJECT

//Rf sensing input pins
uint8_t PinValue0 =0;
uint8_t PinValue1 =0;
uint8_t PinValue2 =0;


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

static const uint8_t PinA0 = 17;  //checks if Gen is on ADC

double genVolt = 0.0;
int sleepTime = 0;
boolean sleepActive = false;
//D3 on logic high flashes the mcu, D4 on high makes it impossible to upload - both keys cannot be used

/*static const uint8_t D8   = 15; Rx
static const uint8_t D9   = 3; Tx
static const uint8_t D10  = 1;*/

//D3 on logic high flashes the mcu, D4 on high makes it impossible to upload - both keys cannot be used

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state


boolean genNowOn= false;

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected to Mahogani_V1.0</h1><a href='/'>Home</a></body></html>");
}

void handleSTATUS() {
  String oN = "nothing";
  String sleep = "Sleep Inactive";
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
    if(startGen())
    server.send(200, "text/html", "<html><body><h1>Gen is now ON</h1><a href='/'>Home</a></body></html>");
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

  pinMode(PinA0, INPUT);
  digitalWrite(Pin7, HIGH);
  digitalWrite(Pin6, HIGH);
  digitalWrite(Pin5, HIGH);
  
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Setting up access point...");
  WiFi.softAP(ssid, password);    //.softAP(const char* ssid, const char* password, int channel, int ssid_hidden, int max_connection)
  //WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Mahogani IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/ON", handleON);
  server.on("/OFF", handleOFF);
  server.on("/SLEEP", handleSLEEP);
  server.on("/STATUS", handleSTATUS);
  server.begin();
  Serial.println("Mahogani server started");
}

void loop() {
  server.handleClient();

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

  //SWITCH NETWORK STATUS
  PinValue2 = digitalRead(Pin2);
  Serial.print("Pin 2 is ");
  Serial.println(PinValue2);
  if(PinValue2 == 1){
    //switch esp network mode - either AP or client
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
  
  delay(1000);
}


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

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
