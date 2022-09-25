#include "wificonfig.h"
#include <ESP8266WiFi.h>

String sbuf;

WiFiServer server(23);
WiFiClient serverClients[5];

wificonfig conf;

void setup() {
  Serial.begin(115200);
  delay(50);
  serverInit();
}

void serverInit() {
  conf.load();
  WiFi.mode(WIFI_STA);
  WiFi.begin(conf.ssid,conf.password);

  long initTime = millis();
  long systTime = initTime;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
    systTime = millis();
    if(systTime - initTime >= 30000) break;
  }
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("connected, address=");
    Serial.println(WiFi.localIP());
    server.begin();
    server.setNoDelay(true);
  } else {
    Serial.println("Unsuccessful, check password and try again.");
  }
}

void serverUpdate(){
  if (WiFi.status() == WL_CONNECTED) {
    if (server.hasClient()) {
      for (int i = 0; i < 5; i++) {
        if (!serverClients[i]) {
          serverClients[i] = server.available();
          Serial.print("New client idx: ");
          Serial.println(i);
          break;
        }
  
        if (i >= 5) {
          server.available().println("no sessions available");
          Serial.print("Server exceeded max connections: ");
          Serial.println(5);
        }
      }
    }
  
    for (int i = 0; i < 5; i++) {
      while (serverClients[i].available()) {
        size_t maxToSerial = std::min(serverClients[i].available(), Serial.availableForWrite());
        maxToSerial = std::min(maxToSerial, (size_t)512);
        uint8_t buf[maxToSerial];
        size_t tcp_got = serverClients[i].read(buf, maxToSerial);
        sbuf = String((char*)buf);
      }
      if(Serial.available() > 0) {
        serverClients[i].println(Serial.readString());
      }
    }
  }
}

void readSerial() {
  if(Serial.available() > 0) {
    // Format should be as such: SSID:PandoraAccessPoint;PASSWORD:password;END;
    String inputMessage = Serial.readString();
    int SSIDPOS = inputMessage.indexOf("SSID:");
    int PASSWORDPOS = inputMessage.indexOf(";PASSWORD:");
    int ENDPOS = inputMessage.indexOf(";END;");

    if (SSIDPOS >= 0 && PASSWORDPOS >= 0 && ENDPOS >= 0) {
      Serial.println("Saving SSID and password");
      conf.ssid = inputMessage.substring(SSIDPOS + 5, PASSWORDPOS);
      conf.password = inputMessage.substring(PASSWORDPOS + 10,ENDPOS);
  
      conf.save();
    }

    Serial.print("SSID: ");
    Serial.println(conf.ssid);
    Serial.print("Password: ");
    Serial.println(conf.password);
  }
}

void loop() {
  readSerial();
  serverUpdate();
}
