#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <FS.h>
#include <FTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

ESP8266WiFiMulti WiFiMulti;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const long utcOffsetInSeconds = 3600;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

const char* ssid     ="MEGATRON";
const char* password = "Psalm121";

//FTP server details
char* xhost = "ftp.iq-joy.com";
char* xusername = "joseph1@eucalyptus.iq-joy.com";
char* xpassword = "Angel1985#20X";
char* xfolder = "/"; 
String fold = "/"+WiFi.macAddress();
char* __dataFileName="";

  
PZEM004Tv30 pzem(D1, D2); // (RX,TX)connect to TX,RX of PZEM
PZEM004Tv30 pzem2(D5, D6);  // (RX,TX) connect to TX,RX of PZEM
PZEM004Tv30 pzem3(D7, D3);  // (RX,TX) connect to TX,RX of PZEM

short FTPresult; //outcome of FTP upload
//Function prototype - required if folder is an optional argument
short doFTP(char* , char* , char* , char* , char* = "");
unsigned long previousMillis = 0;        // will store last time LED was updated
// constants won't change:
const long interval = 10000; 

void setup() {
  digitalWrite(D8, LOW);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    delay(250);
  }
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.println("Connected to Network/SSID");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  SPIFFS.begin();
  /*bool formatted = SPIFFS.format();
  if(formatted){
    Serial.println("\n\nSuccess formatting");
    }else{
        Serial.println("\n\nError formatting");
    }*/
  timeClient.begin();
  
//Open test file, write sample data to it
  fold.replace(":", "");
  __dataFileName[sizeof(fold)];
  fold.toCharArray(__dataFileName, sizeof(__dataFileName));

  //String dateTime = getDateTime();
 // openPowerFileWrite(xhost,xusername,xpassword,__dataFileName, xfolder, dateTime);

}


  int v= 0;
void loop() {

  //unsigned long currentMillis = millis();
  Serial.println(String(currentMillis));
  Serial.println("Ready A:"+String(v++));
  delay(2000);

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    Serial.println(currentMillis);
  previousMillis = currentMillis;
  float voltagex = pzem.voltage();
  float currentx = pzem.current();
  float powerx = pzem.power();
  float energyx = pzem.energy();
  float frequencyx = pzem.frequency();
  float pfx = pzem.pf(); 

  //energymeter 2
  float voltage2 = pzem2.voltage();
  float current2 = pzem2.current();
  float power2 = pzem2.power();
  float energy2 = pzem2.energy();
  float frequency2 = pzem2.frequency();
  float pf2 = pzem2.pf();
 
  // energy meter 3
  float voltage3 = pzem3.voltage();
  float current3 = pzem3.current();
  float power3 = pzem3.power();
  float energy3 = pzem3.energy();
  float frequency3 = pzem3.frequency();
  float pf3 = pzem3.pf();


  //write to file
  String data = "R*: V1="+String(voltagex)+" | A1="+String(currentx)+" | P1="+String(powerx)+" | E1="+String(energyx)
                +" | F1="+String(frequencyx)+" | PF1="+String(pfx)+" Y*: V2="+String(voltage2)+" | A2="+String(current2)
                +" | P2="+String(power2)+" | E2="+String(energy2)
                +" | F2="+String(frequency2)+" | PF2="+String(pf2)+" B*: V3="+String(voltage3)+" | A3="
                +String(current3)+" | P3="+String(power3)+" | E3="+String(energy3)
                +" | F3="+String(frequency3)+" | PF3="+String(pf3);
  Serial.println(data);
  //openPowerFileWrite(xhost,xusername,xpassword,__dataFileName, xfolder, data);
  }
}

String getDateTime(){
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  String timee = timeClient.getFormattedTime();
  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);

  String currentMonthName = months[currentMonth-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  String dateTimee = currentDate+" "+timee;
  return dateTimee;
}


short doFTP(char* host, char* uname, char* pwd, char* fileName, char* folder)
{
  WiFiClient ftpclient;
  WiFiClient ftpdclient;

  const short FTPerrcode = 400; //error codes are > 400
  const byte Bufsize = 128;
  char outBuf[Bufsize];
  short FTPretcode = 0;
  const byte port = 21; //21 is the standard connection port
  Serial.println("Stage 1");
  File ftx = SPIFFS.open(fileName, "r"); //file to be transmitted
  if (!ftx) {
    Serial.println(F("file open failed"));
    return 900;}
  if (ftpclient.connect(host,port)) {
    Serial.println(F("Connected to FTP server"));} 
  else {
    ftx.close();
    Serial.println(F("Failed to connect to FTP server"));
    return 910;}
  FTPretcode = eRcv(ftpclient,outBuf,Bufsize);
  if(FTPretcode >= 400) return FTPretcode;
  
  /* User - Authentication username 
   * Send this command to begin the login process. username should be a 
   * valid username on the system, or "anonymous" to initiate an anonymous login.
   */
  ftpclient.print("USER ");
  ftpclient.println(uname);
  FTPretcode = eRcv(ftpclient,outBuf,Bufsize);
  if(FTPretcode >= 400) return FTPretcode;
   Serial.println("Stage 2");
  /* PASS - Authentication password
   * After sending the USER command, send this command to complete 
   * the login process. (Note, however, that an ACCT command may have to be 
   * used on some systems, not needed with synology diskstation)
   */
  ftpclient.print("PASS ");
  ftpclient.println(pwd);  
  FTPretcode = eRcv(ftpclient,outBuf,Bufsize);
  if(FTPretcode >= 400) return FTPretcode;
 Serial.println("Stage 3");
  //CWD - Change the working folder on the FTP server
  if(!(folder == "")) {
    ftpclient.print("CWD ");
    ftpclient.println(folder);
    FTPretcode = eRcv(ftpclient,outBuf,Bufsize);
    if(FTPretcode >= 400) {return FTPretcode;} }
   Serial.println("Stage 4");
  /* SYST - Returns a word identifying the system, the word "Type:", 
   * and the default transfer type (as would be set by the 
   * TYPE command). For example: UNIX Type: L8 - this is what
   * the diskstation returns
   */
  ftpclient.println("SYST");
  FTPretcode = eRcv(ftpclient,outBuf,Bufsize);
  if(FTPretcode >= 400) return FTPretcode;
   Serial.println("Stage 5");
  /* TYPE - sets the transfer mode
   * A - ASCII text
   * E - EBCDIC text
   * I - image (binary data)
   * L - local format
   * for A & E, second char is:
   * N - Non-print (not destined for printing). This is the default if 
   * second-type-character is omitted
   * Telnet format control (<CR>, <FF>, etc.)
   * C - ASA Carriage Control
   */
  ftpclient.println("Type I");
  FTPretcode = eRcv(ftpclient,outBuf,Bufsize);
  if(FTPretcode >= 400) return FTPretcode;
  
  /* PASV - Enter passive mode
   * Tells the server to enter "passive mode". In passive mode, the server 
   * will wait for the client to establish a connection with it rather than 
   * attempting to connect to a client-specified port. The server will 
   * respond with the address of the port it is listening on, with a message like:
   * 227 Entering Passive Mode (a1,a2,a3,a4,p1,p2), e.g. from diskstation
   * Entering Passive Mode (192,168,0,5,217,101)
   */
  ftpclient.println("PASV");
  FTPretcode = eRcv(ftpclient,outBuf,Bufsize);
  if(FTPretcode >= 400) return FTPretcode;
  /* This is parsing the return from the server
   * where a1.a2.a3.a4 is the IP address and p1*256+p2 is the port number. 
   */
    Serial.println("Stage 6");
  char *tStr = strtok(outBuf,"(,"); //chop the output buffer into tokens based on the delimiters
  int array_pasv[6];
  for ( int i = 0; i < 6; i++) { //there are 6 elements in the address to decode
    tStr = strtok(NULL,"(,"); //1st time in loop 1st token, 2nd time 2nd token, etc.
    array_pasv[i] = atoi(tStr); //convert to int, why atoi - because it ignores any non-numeric chars
                                //after the number
    if(tStr == NULL) {Serial.println(F("Bad PASV Answer"));}
  }
  //extract data port number
  unsigned int hiPort,loPort;
  hiPort=array_pasv[4]<<8; //bit shift left by 8
  loPort=array_pasv[5]&255;//bitwise AND
  Serial.print(F("Data port: "));
  hiPort = hiPort|loPort; //bitwise OR
  Serial.println(hiPort);
   Serial.println("Stage 7");
  //first instance of dftp
  if(ftpdclient.connect(host, hiPort)){Serial.println(F("Data port connected"));}
  else {
    Serial.println(F("Data connection failed"));
    ftpclient.stop();
    ftx.close(); }

  /* STOR - Begin transmission of a file to the remote site. Must be preceded 
   * by either a PORT command or a PASV command so the server knows where 
   * to accept data from
   */
  ftpclient.print("STOR ");
  ftpclient.println(fileName);
  FTPretcode = eRcv(ftpclient,outBuf,Bufsize);
  if(FTPretcode >= 400) {
    ftpdclient.stop();
    return FTPretcode; } 
  Serial.println(F("Writing..."));
  
  byte clientBuf[64];
  int clientCount = 0;
   Serial.println("Stage 8");
  while(ftx.available()) {
    clientBuf[clientCount] = ftx.read();
    clientCount++;
    if(clientCount > 63) {
      ftpdclient.write((const uint8_t *)clientBuf, 64);
      clientCount = 0; }
  }
  if(clientCount > 0) ftpdclient.write((const uint8_t *)clientBuf, clientCount);
  ftpdclient.stop();
  Serial.println(F("Data disconnected"));
  FTPretcode = eRcv(ftpclient,outBuf,Bufsize);
  if(FTPretcode >= 400) {return FTPretcode; } 
   Serial.println("Stage 9");
  //End the connection
  ftpclient.println("QUIT");
  ftpclient.stop();
  Serial.println(F("Disconnected from FTP server"));

  ftx.close();
  Serial.println(F("File closed"));
   Serial.println("Stage 10");
  return FTPretcode;
} // end function doFTP

short eRcv(WiFiClient aclient, char outBuf[], int size)
{
  byte thisByte;
  char index;
  String respStr = "";
  while(!aclient.available()) delay(1);
  index = 0;
  while(aclient.available()) {  
    thisByte = aclient.read();    
    Serial.write(thisByte);
    if(index < (size - 2)) { //less 2 to leave room for null at end
      outBuf[index] = thisByte;
      index++;}
  } //note if return from server is > size it is truncated.
  outBuf[index] = 0; //putting a null because later strtok requires a null-delimited string
  //The first three bytes of outBuf contain the FTP server return code - convert to int.
  for(index = 0; index < 3; index++) {respStr += (char)outBuf[index];}
  return respStr.toInt();
} // end function eRcv

boolean openPowerFileWrite(char* host, char* uname, char* pwd, char* __dataFileName, char* folder, String dateTime){
  File f = SPIFFS.open(__dataFileName, "a+");
  if (!f) {
    Serial.println("file open failed");
    Serial.println("Sketch terminating..."); }
  else {
    Serial.println("file data.txt exists in SPIFFS");
    //write sample data
    f.println(dateTime);
    f.close();
    //get and print contents of root folder
    String str = "";
    Dir dir = SPIFFS.openDir("");
    while (dir.next()) {
      str += dir.fileName();
      str += " / ";
      str += dir.fileSize();
      str += "\r\n";}
    Serial.print(str);
    Serial.println("--------------------------------");

    //Attempt first FTP upload after restart
    
    FTPresult = doFTP(xhost,xusername,xpassword,__dataFileName,xfolder);
     Serial.println("Stage 11");
    //What is the outcome?
    Serial.println("A return code of 226 is success");
    Serial.print("Return code = ");
    Serial.println(FTPresult);
    
    }

}
