#include "wificonfig.h"

wificonfig::wificonfig() {
  SSIDArray = (byte*)malloc(32);
  PASSWORDArray = (byte*)malloc(32);
}

void wificonfig::load() {
  EEPROM.begin(EEPROM_SIZE);
  for(int i = 0; i < 32; i ++){
    byte readEEPROM;
    EEPROM.get(i,readEEPROM);
    SSIDArray[i] = readEEPROM;
  }
  ssid = String((char*)SSIDArray);
  for(int i = 32; i < 64; i ++){
    byte readEEPROM;
    EEPROM.get(i,readEEPROM);
    PASSWORDArray[i - 32] = readEEPROM;
  }

  password = String((char*)PASSWORDArray);
  EEPROM.end();
}

void wificonfig::save() {
  EEPROM.begin(EEPROM_SIZE);
  for(int i = 0; i < ssid.length(); i ++){
    EEPROM.put(i,stringToBytes(ssid,i));
  }
  for(int i = 32; i < (password.length() + 32); i ++){
    EEPROM.put((i),stringToBytes(password,(i - 32)));
  }
  EEPROM.commit();
  EEPROM.end();
}

int wificonfig::stringToBytes(String str, int character) {
  return((byte)str.charAt(character));
}
