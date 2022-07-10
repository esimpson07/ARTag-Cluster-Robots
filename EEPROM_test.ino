#include "wificonfig.h"

wificonfig conf;

long lastSerial = 0;
const long serialTime = 500;

long lastPrint = 0;
const long printTime = 1000;
long cTime = 0;

void setup() {
  Serial.begin(115200);
  delay(500);
  conf.load();

  Serial.print("Starting SSID: ");
  Serial.print(conf.ssid);
  Serial.print(", PW: ");
  Serial.println(conf.password);
}

void readSerial() {
  if(Serial.available() > 0) {
    // Format should be as such: SSID:PandoraAccessPoint;PASSWORD:password;END;
    String inputMessage = Serial.readString();
    int SSIDPOS = inputMessage.indexOf("SSID:");
    int PASSWORDPOS = inputMessage.indexOf(";PASSWORD:");
    int ENDPOS = inputMessage.indexOf(";END;");

    if (SSIDPOS >= 0 && PASSWORDPOS >= 0 && ENDPOS >= 0) {
      Serial.print("Saving SSID and password");
      conf.ssid = inputMessage.substring(SSIDPOS + 5, PASSWORDPOS);
      conf.password = inputMessage.substring(PASSWORDPOS + 10,ENDPOS);
  
      conf.save();
    }
  }
}

void loop() {
  cTime = millis();

  if (cTime > lastSerial + serialTime) {
    lastSerial = cTime;
    
    readSerial();
  }

    
  if (cTime > lastPrint + printTime) {
    lastPrint = cTime;

    conf.load();
    Serial.print("SSID: ");
    Serial.print(conf.ssid);
    Serial.print(", PW: ");
    Serial.println(conf.password);
  }
  delay(10);
}
