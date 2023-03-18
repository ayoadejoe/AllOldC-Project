

void setup() {
  Serial.begin(115200);
  connectWifi();

}

void loop() {
  // put your main code here, to run repeatedly:

}

void connectWifi(){
    Serial.println("Ready");
    Serial.println("");  
    Serial.println("AT+RST", "Ready");
    delay(500);
    SendCommand("AT+CWMODE=3","OK");
    delay(500);
    SendCommand("AT+CIFSR", "OK");
    delay(500);
    SendCommand("AT+CIPMUX=1","OK");
    delay(500);
    SendCommand("AT+CIPSERVER=1,8080","OK"); 
}

