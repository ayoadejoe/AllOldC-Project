float outV = 0;
float resistance = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(" ");
  outV = analogRead(A4);
  Serial.println(outV);
  resistance = 5.0*(outV/1024);
  Serial.println(resistance);
  delay(1000);
}
