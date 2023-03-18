#include <ESP8266WiFi.h>

const char* ssid = "cityWIFIxtra";
const char* password = "2itsthecityy2";

const char* host = "www.example.com";


void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}


void loop()
{
  WiFiClient client;

  if(Serial.available()){
    char url[Serial.available()];
    int a = Serial.available();
    for(int q=0; q<a; q++){
      int c = Serial.read();
      url[q]= (char)c;
    }
    int d = 0;
    while(a>d){
      Serial.print(url[d]);
      d++;
    }
    
    if (client.connect(url, 80))
  {
    Serial.println("connected]");

    Serial.println("[Sending a request]");
    client.print(String("GET /") + " HTTP/1.1\r\n" +
                 "Host: " + url + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );

    Serial.println("[Response:]");
    while (client.connected())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }
  else
  {
    Serial.println("connection failed!]");
    client.stop();
  }
  }
  //Serial.printf("\n[Connecting to %s ... ", host);
  
  delay(15000);
}
