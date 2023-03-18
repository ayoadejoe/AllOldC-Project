#define blueTrig 10
#define blueEcho 9
#define grayTrig 12
#define grayEcho 11
#define red 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  pinMode(blueTrig, OUTPUT);
  pinMode(blueEcho, INPUT);
  pinMode(grayTrig, OUTPUT);
  pinMode(grayEcho, INPUT);
  pinMode(red, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int access = analyse(grayTrig, grayEcho)/10;
  
  Serial.println(access);
  Serial.println(hour());
//  digitalWrite(red, HIGH);
  delay(2000);
 // digitalWrite(red, LOW);
}

int analyse(int trigPin, int echoPin){
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 2.91;
  return distance;
}


