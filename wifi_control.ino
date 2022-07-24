#include <ESP8266WiFi.h>
#define MAX_SRV_CLIENTS 5

String sbuf;

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

const char* ssid = "PandoraAccessPoint";
const char* password = "";

int leftSpeed = 0;
int rightSpeed = 0;

int CMD1 = 0;
int CMD2 = 0;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(5);
  serverInit();
}

void serverInit() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println();
  Serial.print("connected, address=");
  Serial.println(WiFi.localIP());
  server.begin();
  server.setNoDelay(true);
}

void serverUpdate(){
  if (server.hasClient()) {
    for (int i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (!serverClients[i]) {
        serverClients[i] = server.available();
        Serial.print("New client idx: ");
        Serial.println(i);
        break;
      }

      if (i >= MAX_SRV_CLIENTS) {
        server.available().println("no sessions available");
        Serial.print("Server exceeded max connections: ");
        Serial.println(MAX_SRV_CLIENTS);
      }
    }
  }

  for (int i = 0; i < MAX_SRV_CLIENTS; i++) {
    while (serverClients[i].available()) {
      size_t maxToSerial = std::min(serverClients[i].available(), Serial.availableForWrite());
      maxToSerial = std::min(maxToSerial, (size_t)512);
      uint8_t buf[maxToSerial];
      size_t tcp_got = serverClients[i].read(buf, maxToSerial);
      sbuf = String((char*)buf);
      parseComputerString(sbuf);
    }
    if(Serial.available() > 0) {
      serverClients[i].println(Serial.readString());
    }
  }
}

void parseComputerString(String inputMessage) {
  if(inputMessage.substring(0,2) == "C:") {
    int CMD1POS = inputMessage.indexOf("1:");
    int CMD2POS = inputMessage.indexOf("2:");
    int ENDPOS = inputMessage.indexOf("E;");
    
    CMD1 = inputMessage.substring(CMD1POS + 2,CMD2POS).toInt();
    CMD2 = inputMessage.substring(CMD2POS + 2,ENDPOS).toInt();
  } else if(inputMessage.substring(0,2) == "R:") {
    Serial.print(inputMessage);
  }
}

String parseBoardString(String inputMessage) {
  if(inputMessage.substring(0,2) == "M:") {
    return(inputMessage);
  } else {
    return("");
  }
}

void loop() {
  serverUpdate();
}
