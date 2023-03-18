#include <Time.h>  

void setup()  {
  Serial.begin(9600);
}

boolean first = true;
int s = 0;
int u = 0;
void loop(){    
  int tim = second();
  int mint = minute();
  
  Serial.print("Time:");
  Serial.print(mint);

  Serial.print(":");
  Serial.print(tim);
  Serial.println();
  s++;
  delay(5000);
  if(s == 3){
    setTime(0);
  }

  if(mint == 2){
    setTime(0, 7, 9, 0, 0, 0);
  }
}


