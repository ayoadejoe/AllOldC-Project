/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-take-photo-display-web-server/
  
  IMPORTANT!!! 
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include "WiFi.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ESPmDNS.h>

// Replace with your network credentials
const char* ssid = "80DBD5";
const char* password = "asdf;lkj";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket asyncWs("/ws");

boolean takeNewPhoto = false;
camera_fb_t * fb = NULL; // pointer

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/photo.jpg"

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
redacted
</html>)rawliteral";


void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  // TODO: this was all copied from an example and most of it can probably be deleted
  if(type == WS_EVT_CONNECT){
    Serial.printf("ws[%s][%u] connect\n\r", server->url(), client->id());
    //client->printf("Hello Client %u :)", client->id());
    //client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconnect\n\r", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] error(%u): %s\n\r", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n\r", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n\r",msg.c_str());

      if(info->opcode == WS_TEXT) {
        //client->text("I got your text message");
      } else {
        //client->binary("I got your binary message");
      }
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n\r", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n\r", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n\r",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n\r", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n\r", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT) {
            //client->text("I got your text message");
          } else {
            //client->binary("I got your binary message");
          }
        }
      }
    }
  }
}


void connectWifi() {
  // Connect to Wi-Fi
  WiFi.softAP(ssid, password);
  return;

  WiFi.begin(ssid, password);
  for (int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  if (WiFi.status() != WL_CONNECTED) {
    ESP.restart();
  }
  WiFi.setSleep(false);
  Serial.print("original power: ");
  Serial.println(WiFi.getTxPower());
  WiFi.setTxPower(WIFI_POWER_2dBm);
  Serial.print("new power: ");
  Serial.println(WiFi.getTxPower());
}


void setupSpiffs() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }
}


void setupCamera() {
  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Initial framesize defines the size of the buffer
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 4;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
}


void setupHttpRoutes() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = true;
    request->send_P(200, "text/plain", "Taking Photo");
  });

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
  });

  // TODO: refactor this repitition out
  server.on("/saved-photo1", HTTP_GET, [](AsyncWebServerRequest * request) {
    // TODO: don't redirect if we know this is us...?
    // - I think I get 0.0.0.0 for localhost, otherwises this would still work
    /*
    IPAddress serverIp = MDNS.queryHost("camera1");
    Serial.print("camera1: ");
    Serial.println(serverIp);
    if (serverIp == INADDR_NONE) {
      return;
    }
    request->redirect("http://" + serverIp.toString() + "/saved-photo");
    */
    request->redirect("/saved-photo");
  });

  server.on("/saved-photo2", HTTP_GET, [](AsyncWebServerRequest * request) {
    IPAddress serverIp = MDNS.queryHost("camera2");
    Serial.print("camera2: ");
    Serial.println(serverIp);
    if (serverIp == INADDR_NONE) {
      return;
    }
    request->redirect("http://" + serverIp.toString() + "/saved-photo");
    /*
    request->redirect("/saved-photo");
    */
  });

  server.on("/saved-photo3", HTTP_GET, [](AsyncWebServerRequest * request) {
    IPAddress serverIp = MDNS.queryHost("camera3");
    Serial.print("camera3: ");
    Serial.println(serverIp);
    if (serverIp == INADDR_NONE) {
      return;
    }
    request->redirect("http://" + serverIp.toString() + "/saved-photo");
  });

  server.on("/saved-photo4", HTTP_GET, [](AsyncWebServerRequest * request) {
    IPAddress serverIp = MDNS.queryHost("camera4");
    Serial.print("camera4: ");
    Serial.println(serverIp);
    if (serverIp == INADDR_NONE) {
      return;
    }
    request->redirect("http://" + serverIp.toString() + "/saved-photo");
    /*
    request->redirect("/saved-photo");
    */
  });

  server.on("/stream", HTTP_GET, [](AsyncWebServerRequest * request) {
    return;
      Serial.println("Lets stream it up!");

      camera_fb_t * fb = NULL;
      size_t fb_buffer_sent = 0;

      AsyncWebServerResponse *response = request->beginChunkedResponse(_STREAM_CONTENT_TYPE, [fb, fb_buffer_sent](uint8_t *buffer, size_t maxLen, size_t index) mutable -> size_t {
        uint8_t *end_of_buffer = buffer;
        size_t remaining_space = maxLen;

        if (!fb) {
          fb = esp_camera_fb_get();
          if (!fb) {
            Serial.println("Camera capture failed");
            return 0;
          }

          //res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
          size_t len = snprintf((char *)end_of_buffer, remaining_space, _STREAM_BOUNDARY);
          end_of_buffer += len;
          remaining_space -= len;

          size_t hlen = snprintf((char *)end_of_buffer, remaining_space, _STREAM_PART, fb->len);
          end_of_buffer += hlen;
          remaining_space -= hlen;

        }

        //res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
        //TODO: only send max len and later finish sending the buffer
        size_t fb_bytes_to_send = min(remaining_space, fb->len-fb_buffer_sent);
        memcpy((char *)end_of_buffer, fb->buf+fb_buffer_sent, fb_bytes_to_send);
        end_of_buffer += fb_bytes_to_send;
        remaining_space -= fb_bytes_to_send;
        fb_buffer_sent += fb_bytes_to_send;

        if(fb && fb_buffer_sent == fb->len){
          esp_camera_fb_return(fb);
          fb = NULL;
          fb_buffer_sent = 0;
        }

        return maxLen - remaining_space;
      });
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
  });
}


void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  connectWifi();
  setupSpiffs();

  // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  if(!MDNS.begin("camera1")) {
     Serial.println("Error starting mDNS");
     return;
  }
  MDNS.addService("http", "tcp", 80);

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  setupCamera();

  setupHttpRoutes();

  asyncWs.onEvent(onWsEvent);
  server.addHandler(&asyncWs);

  // Start server
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
}

uint8_t counter = 0;
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

  if (takeNewPhoto) {
    capturePhotoSaveSpiffs();
    takeNewPhoto = false;
  } else if (counter % 100 == 0 && !asyncWs.getClients().isEmpty()) {
    streamPhoto();
  }
  counter++;

  asyncWs.cleanupClients();

  delay(3);
}

// Check if photo capture was successful
bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  Serial.print("check size: ");
  Serial.println(pic_sz);
  return ( pic_sz > 100 );
}


bool changeFrameSize(framesize_t frameSize) {
  sensor_t *s = esp_camera_sensor_get();
  if (s == NULL) {
    //return ESP_ERR_CAMERA_NOT_DETECTED;
    return false;
  }

  if (s->status.framesize == frameSize) {
    return false;
  }

  s->set_framesize(s, frameSize);
  //s->set_special_effect(s, frameSize == FRAMESIZE_UXGA ? 1 : 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  Serial.print("Frame size changed to ");
  Serial.println(frameSize);
  delay(400); // It seems to take a while before the new frame size is active. If the photo is taken too early, only the amount of bytes needed to fill the old frame size are captured, though they seem to be captured at the new frame size.

  return true;
}


// Capture Photo and Save it to SPIFFS
esp_err_t capturePhotoSaveSpiffs( void ) {
  fb = NULL; // pointer
  bool ok = false; // Boolean indicating if the picture has been taken correctly

  changeFrameSize(FRAMESIZE_UXGA);

  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return ESP_OK;
    }
    if (fb->len == 0) {
      Serial.println("Frame buffer is empty");
      continue;
    } else {
      Serial.print("Frame buffer size: ");
      Serial.println(fb->len);
      ok = true;
    }
  } while ( !ok );

  ok = false;

  do {
    // Photo file name
    Serial.printf("Picture file name: %s\n\r", FILE_PHOTO);
    //SPIFFS.remove(FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      int result = file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("file.write() result: ");
      Serial.println(result);
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
      if (fb->format == PIXFORMAT_JPEG) {
        Serial.println("fb->format is jpeg");
      } else {
        Serial.println("fb->format is NOT jpeg");
      }
    }
    // Close the file
    file.close();

    // check if file has been correctly saved in SPIFFS
    ok = checkPhoto(SPIFFS);
  } while ( !ok );

  esp_camera_fb_return(fb);

  return ESP_OK;
}


// Capture Photo and Save it to SPIFFS
esp_err_t streamPhoto( void ) {
  fb = NULL; // pointer


  if (changeFrameSize(FRAMESIZE_VGA)) {
    delay(1000); // Not strictly necessary, but if one tries to read the image before it is finished writing to spiffs it seems to get corrupted. Delaying the preview gives a visual cue until this can be automatically prevented. // TODO
  }


  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return ESP_OK;
  }

  if (fb->format == PIXFORMAT_JPEG) {
    //Serial.println("fb->format is jpeg");
  } else {
    Serial.println("fb->format is NOT jpeg");
  }

  //Serial.print("Sending ");
  //Serial.print(fb->len);
  //Serial.println(" bytes of raw jpeg data over websocket");
  AsyncWebSocket::AsyncWebSocketClientLinkedList clients = asyncWs.getClients();
  clients.front()->binary((uint8_t*)fb->buf, fb->len);

  esp_camera_fb_return(fb);

  return ESP_OK;
}
