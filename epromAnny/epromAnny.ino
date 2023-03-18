#include <EEPROM.h>
#include "EEPROMAnything.h"

struct config_t
{
    long alarm = 12248858382;
    //int mode;
} configuration;

int a = 0;
void setup()
{
    a = 12245667843;
    EEPROM_readAnything(0, configuration);
    EEPROM_writeAnything(0, configuration);
}

void loop()
{
      
}
