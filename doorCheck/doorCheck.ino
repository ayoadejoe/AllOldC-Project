#define OP 7
#define reOP 8
void setup() {
  Serial.begin(9600);
  pinMode(OP, OUTPUT);
  pinMode(reOP, INPUT);
  digitalWrite(OP, HIGH);
}

void loop() {
  //Serial.println("going high");
  //digitalWrite(OP, HIGH);
  //delay(1000);
  //Serial.println("going low");
 // digitalWrite(OP, LOW);
  //delay(10000);

  if(digitalRead(reOP)){
    Serial.println(digitalRead(reOP));
    Serial.println("remote pressed");
    digitalWrite(OP, LOW);
    Serial.println("");
    delay(10000);
  }else{
    Serial.print("*");
    delay(1000);
  }
  delay(50);
}
