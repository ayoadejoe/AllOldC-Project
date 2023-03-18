
int A = 4; //Obstacle Sensor A
int B = 6; //Obstacle Sensor B
void setup() {
  Serial.begin(9600);
  pinMode(B,INPUT);
  pinMode(A,INPUT);
}

void loop() {
  int C = digitalRead(A);
  int D = digitalRead(B);
  
  if(C == LOW){
    Serial.println("A is high");
  }

  if(D == LOW){
    Serial.println("B is high");
  }

  delay(2000);
}
