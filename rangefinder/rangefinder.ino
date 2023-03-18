#define blueTrig A1
#define blueEcho A2
#define grayTrig 2
#define grayEcho 3

#define gauge100 A4
#define gauge75 A5
#define red 13
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
int blueLevel = 0;
int grayLevel = 0;
void setup() {
  Serial.begin (9600);
  pinMode(blueTrig, OUTPUT);
  pinMode(blueEcho, INPUT);
  pinMode(grayTrig, OUTPUT);
  pinMode(grayEcho, INPUT);
  
  pinMode(red, OUTPUT);
  pinMode(gauge100, INPUT);
  pinMode(gauge75, INPUT);
}

void loop() {
 
  int access = analyse(grayTrig, grayEcho);
  Serial.print("Average blue Tank:");
  Serial.println(access);
  delay(2000);
  
  int access2 = analyse(blueTrig, blueEcho);
  
  Serial.print("Average gray Tank:");
  Serial.println(access2);
  delay(2000);

  float g75 = gaugeCheck(gauge75);
  Serial.print("75 gauge:");
  Serial.println(g75);

  float g100 = gaugeCheck(gauge100);
  Serial.print("100 gauge:");
  Serial.println(g100);
}

int analyse(int trigPin, int echoPin){
  long duration, distance;  
  cum = 0; int ave = 0;
   
  for(int count = 0; count<10; count++){
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 2.91;
  Serial.print(count);
  Serial.print("> range:");
  Serial.println(distance/10);
  cum+=(distance/10);
  if(count == 9){
    ave = cum/10;
    return ave;
  }
  digitalWrite(red, HIGH);
  delay(1000);
  digitalWrite(red, LOW);
  }
}


float gaugeCheck(int pin){
  Serial.println(" ");
  float outV = analogRead(pin);
  Serial.println(outV);
  float resistance = 5.0*outV;
  Serial.println(resistance);
  return resistance;
}


