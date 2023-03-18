#define trigPin 13
#define echoPin 12
#define red 11
#define grn 10
// gray tank
// - Orange strip - 13 - trigger
// - Blue Strip - 12 - Echo
// - Green  - +
// - Brown  - -

// Blue tank
// - Orange+ Brown Strip = +
// - Blue + Blue Strip = -
// - Brown+Brown Strip+ Orange Strip = Trig
// - Echo = Green Strip
int cum = 0;
int count = 0;
void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(red, OUTPUT);
  pinMode(grn, OUTPUT);

}

void loop() {
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 2.91;
  if (distance < 4) {  // This is where the LED On/Off happens
    digitalWrite(red,HIGH); // When the Red condition is met, the Green LED should turn off
  digitalWrite(grn,LOW);
    } else {
    digitalWrite(red,LOW);
    digitalWrite(grn,HIGH);
  }
  count++;
  cum +=distance;
  if(count == 10){
  int ave = cum/10;
  Serial.print("Average = ");
  Serial.println(ave/10);
  cum = 0;
  count = 0;
  }else{
    Serial.print(distance/10);
    Serial.println(" cm");
  }
  delay(4000);
}
