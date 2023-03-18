#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266AWSIoTMQTTWS.h>  //https://github.com/debsahu/esp8266-arduino-aws-iot-ws
                                  //https://github.com/Links2004/arduinoWebSockets
                                  //https://projects.eclipse.org/projects/technology.paho/downloads (download Arduino version)

const char *ssid = "CityExpress";
const char *password = "128@CITYexpress";

// See `src/aws_iot_config.h` for formatting
char *region = (char *) "eu-west-2";
char *endpoint = (char *) "ajk5h1tv4qdjr-ats";
char *mqttHost = (char *) "ajk5h1tv4qdjr-ats.iot.eu-west-2.amazonaws.com";
int mqttPort = 443;
char *iamKeyId = (char *) "AKIAZUWZUEKCWLBU3IUD";
char *iamSecretKey = (char *) "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";
const char* aws_topic  = "$aws/things/MyEspWork/shadow/update";

ESP8266DateTimeProvider dtp;
AwsIotSigv4 sigv4(&dtp, region, endpoint, mqttHost, mqttPort, iamKeyId, iamSecretKey);
AWSConnectionParams cp(sigv4);
AWSWebSocketClientAdapter adapter(cp);
AWSMqttClient client(adapter, cp);

void setup() {

    Serial.begin(115200);
    while(!Serial) {
        yield();
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    if(WiFi.status() == WL_CONNECTED){
      Serial.println("Connected to WIfi");
    }
    int res = client.connect();
    Serial.printf("mqtt connect=%d\n", res);

    if (res == 0) {
      client.subscribe(aws_topic, 1,
        [](const char* topic, const char* msg)
        { Serial.printf("Got msg '%s' on topic %s\n", msg, topic); }
      );
    }
}

void loop() {
  if (client.isConnected()) {
    DynamicJsonDocument doc(4096);
    doc["state"]["reported"]["value"] = random(100);
    serializeJson(doc, Serial);
    Serial.println();
    char shadow[measureJson(doc) + 1];
    serializeJson(doc, shadow, sizeof(shadow));

    client.publish(aws_topic, shadow, 0, false);
    client.yield();

  } else {
    Serial.println("Not connected...");
    delay(2000);
    int res = client.connect();
    Serial.printf("mqtt connect=%d\n", res);
  }

  delay(30000);
}
