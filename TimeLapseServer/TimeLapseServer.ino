/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-pir-motion-detector-photo-capture/
 
  IMPORTANT!!!
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/
 #include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
// define the number of bytes you want to access
#define EEPROM_SIZE 1

int capture_interval = 1000; // microseconds between captures
// Replace with your network credentials
const char* ssid = "Transmission";
const char* password = "John3:16";

bool busy = false;
long current_millis;
long last_capture_millis = 0;
// Create AsyncWebServer object on port 80
const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>

<html lang="en">
<head>
  <meta charset="utf-8">
  <title>IQ-JOY's Eucalyptus</title>
  <meta name="Real-Time Monitoring device coded by Ayoade Adetunji" content="Eucalyptus Server Code">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
    
<style>

body {
  background-color: #faf2e4;
  margin: 0 15%;
  font-family: sans-serif;
  }

h1 {
  text-align: center;
  font-family: serif;
  font-weight: normal;
  text-transform: uppercase;
  border-bottom: 1px solid #57b1dc;
  margin-top: 30px;
}

h2 {
  color: #d1633c;
  font-size: 1em;
}

h3 {
  color: #09c1dc;
  font-size: 0.8em;
  border-bottom: 1px solid #09c1dc;
}
    
.center {
  display: block;
  margin: auto;
  width: 50%;
  height: 50%;
}

.button {
        display: inline-block;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        color: #ffffff;
        background-color: #7aa8b7;
        border-radius: 6px;
        outline: none;
      }

figure {
  border: 1px #cccccc solid;
  padding: 4px;
  margin: auto;
}

figcaption {
  background-color: black;
  color: white;
  font-style: italic;
  padding: 2px;
  text-align: center;
}

.flex-container {
    display: flex;
}

.flex-child {
    flex: 1;
}  

.flex-child:first-child {
    margin-right: 20px;
} 

</style>
</head>

<body>
<h1><img src="https://iq-joy.com/wp/wp-content/uploads/2020/12/cropped-IQJOY_logo-1.png" alt="Black Goose logo"><br>Eucalyptus smart cctv </h1>
    
<p>HERE ARE YOUR DOUBTS</p>
    
<br><br>  
<div class="flex-container">

  <div class="flex-child magenta">
    ~PLACEHOLDER_CAPTURED_IMAGE~
  </div>
  
  <div class="flex-child green">
  
     <figure>
      <img src="https://i-margin.com/wp-content/uploads/2019/09/intru.jpg" alt="captured image" style="width:100%" id="selectedImg">
  <figcaption>Captured Image</figcaption>
    </figure>

  </div>
  
</div>
    
<br><br>

<script>
function changeImage(x) {
    var t = document.getElementById('selectedImg')
    var mySrc = t.getAttribute('');
    var img = "/clicked?photo="+x.id;
    console.log("img:"+img);
    if( mySrc == img ){
        t.setAttribute('src',img);
        t.setAttribute('alt',img);
        } else {
        t.setAttribute('src',img);
        t.setAttribute('alt',img);
        }
 }
</script>

</body>
</html>)rawliteral";

String getPicFiles(fs::FS &fs, const char * dirname, uint8_t levels){
    busy = true;
    String items ="";
    int p=0;
    File root = fs.open("/");
    if(!root){
        Serial.println("Failed to open directory");
      }
      if(!root.isDirectory()){
        Serial.println("Not a directory");
      }
    

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
          TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
           TIMERG0.wdt_feed=1;
           TIMERG0.wdt_wprotect=0;

           Serial.println(" - not a directory");
        } else {
          TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
          TIMERG0.wdt_feed=1;
          TIMERG0.wdt_wprotect=0;

            String fileName = file.name();
            Serial.println("File:"+fileName);
            if(fileName.startsWith( String("/IMG"))){
             // <h3><a onclick="changeImage(this)" id="">Saved Photo1 </a></h3>
              fileName.replace("/", "");
              items+= "<h3><a onclick=\"changeImage(this)\" id="+fileName+"> "+fileName+"</a></h3> ";
              p++;
            }
        }
        file = root.openNextFile();
    }
    busy = false;
    return items;
}

String processor(const String& var){
  Serial.println("var received:"+var);
 
  if(var == "PLACEHOLDER_CAPTURED_IMAGE"){
   String images = "<div class=\"fixed\"> ";
   fs::FS &fs = SD_MMC;
    String items = getPicFiles(fs, "/", 0);
    images+=items;
    images+=" </div>";
    Serial.println("List of Images:"+ images);
    return images;
  }
 
  return String();
}


 
RTC_DATA_ATTR int bootCount = 0;

// Pin definition for CAMERA_MODEL_AI_THINKER
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
 
int pictureNumber = 0;


void initWiFi() {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}



void takePicture(){
  int pictureNumber = EEPROM.read(0) + 1;
  //take new image
  camera_fb_t * fb = NULL;
  //obtain camera frame buffer
  fb = esp_camera_fb_get();
  if (!fb) 
  {
    Serial.println("Camera capture failed");
    Serial.println("Exiting now"); 
    while(1);   //wait here as something is not right
  }

  //save to SD card
  //generate file path
  String path = "/IMG" + String(pictureNumber) + ".jpg";
    
  fs::FS &fs = SD_MMC;

  //create new file
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file)
  {
    Serial.println("Failed to create file");
    Serial.println("Exiting now"); 
    while(1);   //wait here as something is not right    
  } 
  else 
  {
    file.write(fb->buf, fb->len); 
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file.close();

  //return camera frame buffer
  esp_camera_fb_return(fb);
  Serial.printf("Image saved: %s\n", path.c_str());

}
  
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);
 
  Serial.setDebugOutput(true);
 
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
  
  pinMode(4, INPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_dis(GPIO_NUM_4);
 
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
 
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
 
  Serial.println("Starting SD Card");
 
  delay(500);
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    //return;
  }
 
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }
   
  camera_fb_t * fb = NULL;
 
  // Take Picture with Camera
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;
 
  // Path where new picture will be saved in SD Card
  String path = "/IMG" + String(pictureNumber) +".jpg";
 
  fs::FS &fs = SD_MMC;
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  
  Serial.printf("Picture file name: %s\n", path.c_str());
 
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
  }
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file.close();
  esp_camera_fb_return(fb);
  
  delay(1000);
  
  /* Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);
*/
 initWiFi();


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
} 
 
void loop() {
 if(!busy){
  current_millis = millis();
  if (current_millis - last_capture_millis > capture_interval) { // Take another picture
    last_capture_millis = millis();
    takePicture();
  }
  }else{
    
  }

}
