#include <EEPROM.h>

#define EEPROM_SIZE 20

String EEPROM_SSID;
String EEPROM_PASSWORD;

byte* SSIDArray;
byte* PASSWORDArray;

void setup() {
  SSIDArray = (byte*)malloc(32);
  PASSWORDArray = (byte*)malloc(32);
  Serial.begin(115200);
}

void readSerial() {
  if(Serial.available() > 0) {
    // Format should be as such: SSID:PandoraAccessPoint;PASSWORD:password;END;
    String inputMessage = Serial.readString();
    int SSIDPOS = inputMessage.indexOf("SSID:");
    int PASSWORDPOS = inputMessage.indexOf(";PASSWORD:");
    int ENDPOS = inputMessage.indexOf(";END;");
    
    EEPROM_SSID = inputMessage.substring(SSIDPOS + 5, PASSWORDPOS);
    EEPROM_PASSWORD = inputMessage.substring(PASSWORDPOS + 10,ENDPOS);
  }
}

void commitEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  Serial.println("EEPROM opened successfully for writing.");
  Serial.println(EEPROM_SSID);
  for(int i = 0; i < EEPROM_SSID.length(); i ++){
    Serial.print("EEPROM SSID byte put! i = ");
    Serial.println(stringToBytes(EEPROM_SSID,i));
    EEPROM.put(i,stringToBytes(EEPROM_SSID,i));
  }
  for(int i = 32; i < (EEPROM_PASSWORD.length() + 32); i ++){
    Serial.print("EEPROM PASSWORD byte put! i = ");
    Serial.println(stringToBytes(EEPROM_PASSWORD,(i - 32)));
    EEPROM.put(i,stringToBytes(EEPROM_PASSWORD,(i - 32)));
  }
  EEPROM.commit();
  EEPROM.end();
}

void readEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  for(int i = 0; i < 32; i ++){
    byte readEEPROM;
    EEPROM.get(i,readEEPROM);
    SSIDArray[i] = readEEPROM;
    Serial.println(readEEPROM);
  }
  for(int i = 32; i < 64; i ++){
    byte readEEPROM;
    EEPROM.get(i,readEEPROM);
    PASSWORDArray[i - 32] = readEEPROM;
    Serial.print(i);
    Serial.println(readEEPROM);
  }
  EEPROM.end();
}

int stringToBytes(String str, int character) {
  return((byte)str.charAt(character));
}

void loop() {
  EEPROM_SSID = "accesspoint";
  EEPROM_PASSWORD = "password";
  commitEEPROM();
  readEEPROM();
  Serial.println(String((char*)SSIDArray));
  Serial.println(String((char*)PASSWORDArray));
  delay(10000);
}
