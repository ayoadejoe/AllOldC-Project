



/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-video-streaming-web-server-camera-home-assistant/
  
  IMPORTANT!!! 
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include "esp_http_server.h"
#include <HTTPClient.h>
#include <ESP32_FTPClient.h>
#include "octocat.h"
#include "SPIFFS.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32

#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
// define the number of bytes you want to access
#define EEPROM_SIZE 1
int pictureNumber = 0;
boolean telegram = false;     //if you do not want to use telegram
boolean web = true;           //if you want to use web

 
RTC_DATA_ATTR int bootCount = 0;

//Replace with your network credentials
const char* ssid = "MEGATRON";
const char* password = "Psalm121";
char ftp_server[] = "ftp.iq-joy.com";
char ftp_user[]   = "joseph1@eucalyptus.iq-joy.com";
char ftp_pass[]   = "Angel1985#20X";

// you can pass a FTP timeout and debbug mode on the last 2 arguments
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass, 5000, 2);

#define PART_BOUNDARY "123456789000000000000987654321"

// This project was tested with the AI Thinker Model, M5STACK PSRAM Model and M5STACK WITHOUT PSRAM
#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM

// Not tested with this model
//#define CAMERA_MODEL_WROVER_KIT

#if defined(CAMERA_MODEL_WROVER_KIT)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    21
  #define SIOD_GPIO_NUM    26
  #define SIOC_GPIO_NUM    27
  
  #define Y9_GPIO_NUM      35
  #define Y8_GPIO_NUM      34
  #define Y7_GPIO_NUM      39
  #define Y6_GPIO_NUM      36
  #define Y5_GPIO_NUM      19
  #define Y4_GPIO_NUM      18
  #define Y3_GPIO_NUM       5
  #define Y2_GPIO_NUM       4
  #define VSYNC_GPIO_NUM   25
  #define HREF_GPIO_NUM    23
  #define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_M5STACK_WITHOUT_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       17
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_AI_THINKER)
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
#else
  #error "Camera model not selected"
#endif

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t capture_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

int gpioPIR = 15;   //PIR Motion Sensor
IPAddress local_IP(192,168,8,11);
// Set your Gateway IP address
IPAddress gateway(192, 168, 8, 1);

IPAddress subnet(255,255,255,0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

const char* host = "checkip.amazonaws.com";
String getNetworPublicIP = "http://checkip.amazonaws.com";

String serverName = "http://eucalyptus.iq-joy.com/joseph1/eucabasic1.php";

String payload = "";
String payload2 = "";
String payload3 = "";
String espMacID = "";
String espIP = "";

String token = "1515348043:AAHlREypfhZeCTuv3xEmUzCs6H4Q8VZwwhI";
String chat_id = "546145510";

int counter = 0;
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

boolean gotPublicIP = false;
boolean streamInProgress = false;
uint8_t lastFileNo=0;

static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    return res;
  }

  while(true){
    streamInProgress = true;
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if(fb->width > 400){
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if(res != ESP_OK){
      break;
    }
    //Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
  }

  streamInProgress = false;
  return res;
}

//handle get still pic from http://ip/capture 
boolean ok = false;

static esp_err_t capture_handler(httpd_req_t *req){   

    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
   
    fb = esp_camera_fb_get(); //get picture from cam 
    if (!fb) {
        Serial.println("Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");

    if( fb->width > 400){
        Serial.println("Taking picture now!");
        size_t fb_len = 0;
 
        if(fb->format == PIXFORMAT_JPEG){
            Serial.println("Sending to server on port 80");
            fb_len = fb->len;
            res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
        } 
    }
    
    esp_camera_fb_return(fb);
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        formatSPIFF();
    }

    if(setupSpiffs()){
      saveToSPIFF(fb);
    }
    
    return res;
}

void formatSPIFF(){
  bool formatted = SPIFFS.format();
         
          if(formatted){
            Serial.println("\n\nSuccess formatting");
          }else{
            Serial.println("\n\nError formatting");
          }
          delay(3000);
          ESP.restart();
}

boolean setupSpiffs() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
    return false;
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");

/*
    File file = SPIFFS.open("/test.txt", FILE_WRITE);
 
    if(!file){
        Serial.println("Failed to open file for reading");
    }else{
      if(file.print("TEST WORK")) {
      Serial.println("File was written");
      }else {
          Serial.println("File write failed");
      }
    }
 
    Serial.println("File Content:");
    */
    return true;
  }
}

boolean picNoRec = false;
void saveToSPIFF(camera_fb_t * fb){  
  EEPROM.begin(EEPROM_SIZE);
  if(!picNoRec){  //if it is first login, the last picture no mark is recalled
    lastFileNo = EEPROM.read(0);
    picNoRec = true;
  }
  pictureNumber = EEPROM.read(0) + 1;
  if(pictureNumber>50){
    pictureNumber = 1;
    EEPROM.write(0, pictureNumber);
    delay(100);
    lastFileNo = EEPROM.read(0);
    picNoRec = true;
  }
  Serial.println("Picture no now at:"+String(pictureNumber));
  // Path where new picture will be saved in SD Card
  String picName = "/picture" + String(pictureNumber) +".jpg";
  Serial.println("Picture name:"+picName);
  
  File file = SPIFFS.open(picName, FILE_WRITE);

  ok = false;
  
  do{
    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
       Serial.println("An Error has occurred while mounting SPIFFS");
      formatSPIFF();
    }
    else {
      int result = file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("file.write() result: ");
      Serial.println(result);
      Serial.print("The picture has been saved in ");
      Serial.print(picName);
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
    ok = checkPhoto(SPIFFS, picName);
  }while(!ok);

  EEPROM.write(0, pictureNumber);
  EEPROM.commit();
    Serial.print("pic File is "+ok);
}

// Check if photo capture was successful
bool checkPhoto( fs::FS &fs, String picName) {
  File f_pic = fs.open( picName );
  unsigned int pic_sz = f_pic.size();
  Serial.print("check size: ");
  Serial.println(pic_sz);
  return ( pic_sz > 100 );
}


void startCameraServer(){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;//overwrite

    httpd_uri_t capture_uri = {
        .uri       = "/capture",
        .method    = HTTP_GET,
        .handler   = capture_handler,
        .user_ctx  = NULL
    };

   httpd_uri_t stream_uri = {
        .uri       = "/stream",
        .method    = HTTP_GET,
        .handler   = stream_handler,
        .user_ctx  = NULL
    };

    Serial.printf("Starting stream server on port: '%d'\n", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
    

    config.server_port = 8080;//overwrite
    config.ctrl_port = 8080;

    Serial.printf("Starting web server on port: '%d'\n", config.server_port);

    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(camera_httpd, &capture_uri);
    }
     
    

}



void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  Serial.begin(115200);
  //Serial.setDebugOutput(false);

  delay(10);
   
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
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

    // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }


 sensor_t * s = esp_camera_sensor_get();
 s->set_framesize(s, FRAMESIZE_VGA);  
 s->set_contrast(s, 2);       // -2 to 2
 s->set_saturation(s, -2);     // -2 to 2
  
  
  // Wi-Fi connection
  WiFi.mode(WIFI_AP_STA); 
  WiFi.softAP("EUCACAM", "John3:16");  //Start HOTspot removing password will disable security

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }else{
    Serial.println("Static IP assigned");
  }
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.print(WiFi.localIP());

//signal that you are connected by flashin built-in led
    if (WiFi.status() != WL_CONNECTED) {
     Serial.println("Reset");
    
    ledcAttachPin(4, 3);
    ledcSetup(3, 5000, 8);
    ledcWrite(3,10);
    delay(200);
    ledcWrite(3,0);
    delay(200);    
    ledcDetachPin(3);
    delay(1000);
    ESP.restart();
  }else 
  {
    ledcAttachPin(4, 3);
    ledcSetup(3, 5000, 8);
    for (int i=0;i<5;i++) {
      ledcWrite(3,10);
      delay(200);
      ledcWrite(3,0);
      delay(200);    
    }
    ledcDetachPin(3);      
  }
  // Start streaming web server
  startCameraServer();
  
  espIP = "192,168,8,10";
  espMacID = WiFi.macAddress();

}

void loop() {
  if(!gotPublicIP){
    Serial.println("Trying to get Public IP");
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
    
          HTTPClient http;
       
          http.begin("http://checkip.amazonaws.com/"); //Specify the URL
          int httpCode = http.GET();                                        //Make the request
       
          if (httpCode > 0) { //Check for the returning code
       
              payload = http.getString();
              Serial.println(httpCode);
              Serial.println(payload);
              gotPublicIP=true;
            }else {
            Serial.println("Error on HTTP request");
          }

          if(gotPublicIP){
            HTTPClient http2;
            payload.trim();
            String serverPath = serverName + "?ESPName="+espMacID+"&IP="+payload;
            Serial.print("serverPath:");
            Serial.println(serverPath);
            
            http2.begin(serverPath.c_str());
            http.setUserAgent("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0");
            int httpCode2 = http2.GET();                                        //Make the request
       
            if (httpCode2 > 0) { //Check for the returning code
                payload2 = http2.getString();
                Serial.println(httpCode2);
                Serial.println(payload2);
              }else {
              Serial.println("Error on HTTP request");
            }
          }

          
          http.end(); //Free the resources
          }
          else {
            Serial.println("WiFi Disconnected");
          }
          lastTime = millis();
        }
        delay(10000);
        }
    else{
        counter++;
        Serial.print("Public IP:");
        Serial.println(payload);

        Serial.print("Local IP:");
        Serial.println(espIP);

        Serial.print("ESP Name:");
        Serial.println(espMacID);


        if(counter>50){
          if(gotPublicIP){
            HTTPClient http3;
            payload.trim();
            String url = "http://eucalyptus.iq-joy.com/joseph1/eucabasic1.php?ESPName="+espMacID+"&IP="+payload;
            Serial.print("url:");
            Serial.println(url);
           
            http3.begin(url); //Specify the URL
            int httpCode3 = http3.GET();                                        //Make the request
       
            if (httpCode3 > 0) { //Check for the returning code
                payload3 = http3.getString();
                Serial.println(httpCode3);
                Serial.println(payload3);
              }else {
              Serial.println("Error on HTTP request");
            }

            //uploadSPIFFImages();
          }
          counter =0;
        }

        if(!streamInProgress){
          Serial.println("Checking Motion");
          pinMode(gpioPIR, INPUT_PULLUP);
          int v = digitalRead(gpioPIR);
          Serial.println(v);
          if (v==0){
              camera_fb_t * fb = NULL;
              fb = esp_camera_fb_get();  
              if(!fb) {
                Serial.println("Camera capture failed");
                delay(1000);
                ESP.restart();
                 Serial.println("Camera capture failed");
              }  

            if(telegram){
              alerts2Telegram(token, chat_id, fb);
              delay(2000);
            }
             
            if(web){
              ftpUpload(fb, espMacID);
            }
            
          }
        }
       delay(1000); 
    }
}


void uploadSPIFFImages(){
  
  if(lastFileNo<pictureNumber){
     int z = lastFileNo;
     Serial.println("lastfileno: " + String(lastFileNo));
     
     for( uint8_t k = lastFileNo; k <=pictureNumber; k++){
      
        ftp.OpenConnection();
          Serial.println("pictureno: " + String(pictureNumber));
          String fille = "/picture" + String(k) +".jpg";
          String refille = "picture" + String(k) +".jpg";
          Serial.println("fille: " + fille);
          const char * fileName =  refille.c_str();
          
          unsigned int fileSize = 0;

        File file2 = SPIFFS.open(fille);
    
       
        if(!file2){
            Serial.println("Failed to open file for reading");
            return;
        }else{
          fileSize = file2.size();
           // Print file size
            Serial.println("\nFile size is: " + String(fileSize));
        }

      if(fileSize >0){
        //Dynammically alocate buffer
        unsigned char * image_file = (unsigned char *) malloc(fileSize);
        Serial.println("fileSize sent: " + fileSize);


        ftp.InitFile("Type I");
        ftp.NewFile( fileName );
        
        ftp.WriteData(image_file, fileSize);
        ftp.CloseFile();
        file2.close();
        Serial.println("FTP write completed");
      }
      ftp.CloseConnection();
   }

   lastFileNo = pictureNumber;
   delay(1000);
  }
  
}

boolean dirAvailable = false;
void ftpUpload(camera_fb_t * fb, String macID){
              
             EEPROM.begin(EEPROM_SIZE);
              if(!picNoRec){  //if it is first login, the last picture no mark is recalled
                lastFileNo = EEPROM.read(0);
                picNoRec = true;
              }
              pictureNumber = EEPROM.read(0) + 1;
              EEPROM.write(0, pictureNumber);
              EEPROM.commit();

                
            uint8_t *fbBuf = fb->buf;
            size_t fbLen = fb->len;
            Serial.println("*fbBuf: " + String(*fbBuf));
            Serial.println("fbLen: " + String(fbLen));
            
            ftp.OpenConnection();
            String picName = "picture" + String(pictureNumber) +".jpg";
            Serial.println("picName: " + picName);
            
            String direc = "ESP32_"+macID;
            // Get directory content
            ftp.InitFile("Type A");
            String list[128];
            ftp.ContentList("", list);
            Serial.println("\nDirectory info: ");
            
            for(int i = 0; i < sizeof(list); i++){
              
              if(list[i].length() > 0){
                
              String dirList = list[i];
              Serial.println(dirList);
                if(dirList.indexOf(direc) > 0){
                  dirAvailable = true;
                  Serial.println("Directory is available");
                  break;
                }
              
              } else
                break;
                
            }

           
             // Make a new directory
            if(!dirAvailable){
              ftp.InitFile("Type A");
              ftp.MakeDir(direc.c_str());
            }
            // Create the new file and send the image
            ftp.ChangeWorkDir(direc.c_str());
            
            ftp.InitFile("Type I");
            ftp.NewFile( picName.c_str() );
            
            ftp.WriteData( fb->buf, fbLen);
            ftp.CloseFile();
            Serial.println("FTP write completed");
          
            ftp.CloseConnection();
}


String alerts2Telegram(String token, String chat_id, camera_fb_t * fb) 
{
  const char* myDomain = "api.telegram.org";
  String getAll="", getBody = "";

  

WiFiClientSecure client_tcp;
  
  if (client_tcp.connect(myDomain, 443)) 
{
    Serial.println("Connected to " + String(myDomain));
    
    String head = "--India\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + chat_id + "\r\n--India\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--India--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;
  
    client_tcp.println("POST /bot"+token+"/sendPhoto HTTP/1.1");
    client_tcp.println("Host: " + String(myDomain));
    client_tcp.println("Content-Length: " + String(totalLen));
    client_tcp.println("Content-Type: multipart/form-data; boundary=India");
    client_tcp.println();
    client_tcp.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;


    for (size_t n=0;n<fbLen;n=n+1024)
 {

      if (n+1024<fbLen) 
{
        client_tcp.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) 
{
        size_t remainder = fbLen%1024;
        client_tcp.write(fbBuf, remainder);
      }
    }  
    
    client_tcp.print(tail);
    
    esp_camera_fb_return(fb);
    
    int waitTime = 10000;   // timeout 10 seconds
    long startTime = millis();
    boolean state = false;
    
    while ((startTime + waitTime) > millis())
    {
      Serial.print(".");
      delay(100);      
      while (client_tcp.available()) 
      {
          char c = client_tcp.read();
          if (c == '\n') 
          {
            if (getAll.length()==0) state=true; 
            getAll = "";
          } 
          else if (c != '\r')
            getAll += String(c);
          if (state==true) getBody += String(c);
          startTime = millis();
       }
       if (getBody.length()>0) break;
    }
    client_tcp.stop();
    Serial.println(getBody);
  }
  else {
    getBody = "Connection to telegram failed.";
    Serial.println("Connection to telegram failed.");
  }
  
  return getBody;
}
