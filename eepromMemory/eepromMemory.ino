
#include <EEPROM.h>
#include <Time.h>  
/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
int addr = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {

delay(5000);

addr= 0;

while(addr <8){
  int val = EEPROM.read(addr);
  Serial.println(val, DEC);
  addr++;
}
delay(1000);
}
