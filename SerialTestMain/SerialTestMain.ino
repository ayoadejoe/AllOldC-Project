int incomingByte = 0; // for incoming serial data
int incomingByte1 = 0; // for incoming serial data
int incomingByte2 = 0; // for incoming serial data


boolean first = false;

void setup() {
  Serial.begin(115200); // opens serial port, sets data rate to 9600 bps
  Serial1.begin(115200);
  Serial2.begin(115200);
  
}

void loop() {

  if(!first){
    Serial.println("Ready Serial one");
    Serial1.println("Ready Serial two");
    Serial2.println("Ready Serial three");
    first = true;
  }
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("Serial 1 received: ");
    Serial.println(incomingByte, DEC);
    first = false;
  }

  if (Serial1.available() > 0) {
    // read the incoming byte:
    incomingByte1 = Serial1.read();

    // say what you got:
    Serial1.print("Serial 2 received: ");
    Serial1.println(incomingByte1, DEC);
    first = false;
  }

  if (Serial2.available() > 0) {
    // read the incoming byte:
    incomingByte2 = Serial2.read();

    // say what you got:
    Serial2.print("Serial3 received: ");
    Serial2.println(incomingByte2, DEC);
    first = false;
  }
}
