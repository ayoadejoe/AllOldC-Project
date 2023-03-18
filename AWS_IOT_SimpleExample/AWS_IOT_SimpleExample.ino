#include <ESP8266WiFi.h>
#include <AmazonIOTClient.h>
#include "ESP8266AWSImplementations.h"

Esp8266HttpClient httpClient;
Esp8266DateTimeProvider dateTimeProvider;

AmazonIOTClient iotClient;
ActionError actionError;

#include <WiFiClientSecureBearSSL.h>
// Fingerprint for demo URL, expires on June 2, 2021, needs to be updated well before this date
const uint8_t fingerprint[20] = {0x40, 0xaf, 0x00, 0x6b, 0xec, 0x90, 0x22, 0x41, 0x8e, 0xa3, 0xad, 0xfa, 0x1a, 0xe8, 0x25, 0x41, 0x1d, 0x1a, 0x54, 0xb3};


//Station mode you should put your ssid and password
const char *ssid = "CityExpress";
const char *password = "128@CITYexpress";
/*
static IPAddress ip(192, 168, 8, 101); // static IP used for browser access: http://IPaddress
static IPAddress gateway(192, 168, 8, 1);
static IPAddress subnet(255, 255, 255, 0);
*/

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WAP
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerprint);
    delay(500);
    Serial.print(".");
  }

 // WiFi.config(ip, gateway, subnet); // remove this line to use DHCP
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  iotClient.setAWSRegion("eu-west-2");
  iotClient.setAWSEndpoint("ajk5h1tv4qdjr-ats");
  //iotClient.setAWSDomain("arn:aws:iot:eu-west-2.amazonaws.com");
  iotClient.setAWSDomain("ajk5h1tv4qdjr-ats.iot.eu-west-2.amazonaws.com");
  iotClient.setAWSPath("/things/MyEspWork/shadow/");
  iotClient.setAWSKeyID("AKIAZUWZUEKCWLBU3IUD");
  iotClient.setAWSSecretKey("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");
  iotClient.setHttpClient(&httpClient);
  iotClient.setDateTimeProvider(&dateTimeProvider);
}

void loop(){
  char* shadow = "{\"state\":{\"reported\": {\"foobar\": \"bar\"}}}";

  char* result = iotClient.update_shadow(shadow, actionError);
  Serial.println("ACTION ERROR:"+actionError);
  Serial.println(result);

  delay(60000);
}
