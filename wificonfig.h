#include <EEPROM.h>
#include "Arduino.h"

#define EEPROM_SIZE 64

class wificonfig {
  public:
    wificonfig();
    
    String ssid;
    String password;

    void load();
    void save();
  private:
    byte* SSIDArray;
    byte* PASSWORDArray;

    int stringToBytes(String str, int character);
};
