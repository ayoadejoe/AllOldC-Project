#define OP A1

void setup() {
  Serial.begin(9600);
  pinMode(OP, INPUT);
}

void loop() {
  int d = analogRead(OP);
  Serial.println(d);
  delay(1000);
}
