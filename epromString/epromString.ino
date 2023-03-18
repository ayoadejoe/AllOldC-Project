void setup() {
  Serial.begin(9600);

}

int x =0;
String rec = "gogo";
void loop() {
  Serial.println(rec);
  
  while(Serial.available()>0){
    rec = Serial.readString();
    x=0;
  }
  
  while (x<rec.length()){
    char it = rec.charAt(x);
    int d = (int)it;
    Serial.print(d);
    Serial.print(" - ");
    Serial.println((char)d);
    x++;
  }
  delay(1000);
}
