#include <ESP8266WiFi.h>

#define servoDriver 0x40
#define SERVERMODE 0
#define BAUD_SERIAL 115200
#define RXBUFFERSIZE 1024
#define STACK_PROTECTOR  512
#define MAX_SRV_CLIENTS 5

#define ENA D2
#define IN1 D3
#define IN2 D4
#define IN3 D7
#define IN4 D8
#define ENB D6

int leftSpeed = 0;
int rightSpeed = 0;

String sbuf;
WiFiServer server(23); // the port the telnet connection is on
WiFiClient serverClients[MAX_SRV_CLIENTS];

#if SERVERMODE
const char* ssid = "None";
const char* password = "None";
#else
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
String id = "ESPsoftAP_10";
String pw = "Soft_AP_PW";
#endif

void setup() {
  pinMode(ENA,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENB,OUTPUT);
  
  Serial.begin(115200);
  
  #if SERVERMODE
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println();
  Serial.print("connected, address=");
  Serial.println(WiFi.localIP());
  #else
  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP(id, pw);
  if(result == true)
  {
    WiFi.softAPConfig(local_ip, gateway, subnet);
    Serial.println("Wifi Ready");
  } else {
    Serial.print("WiFi Failed: ");
    Serial.println(result);
  }
  #endif
  server.begin();
  server.setNoDelay(true);
}

void loop() {
  leftDrive(leftSpeed);
  rightDrive(rightSpeed);
  serverUpdate();
}

void leftDrive(int driveSpeed) {
  if(driveSpeed > 0) {
    analogWrite(ENA,abs(driveSpeed));
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
  } else if(driveSpeed < 0) {
    analogWrite(ENA,abs(driveSpeed));
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
  } else {
    analogWrite(ENA,0);
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,HIGH);
  }
}

void rightDrive(int driveSpeed) {
  if(driveSpeed > 0) {
    analogWrite(ENB,abs(driveSpeed));
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
  } else if(driveSpeed < 0) {
    analogWrite(ENB,abs(driveSpeed));
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
  } else {
    analogWrite(ENB,0);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,HIGH);
  }
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
      maxToSerial = std::min(maxToSerial, (size_t)STACK_PROTECTOR);
      uint8_t buf[maxToSerial];
      size_t tcp_got = serverClients[i].read(buf, maxToSerial);

      sbuf = String((char*)buf);
      leftSpeed = sbuf.substring(0,3).toInt() - 255;
      rightSpeed = sbuf.substring(3,6).toInt() - 255;
      Serial.println(leftSpeed);
      Serial.println(rightSpeed);
    }
  }
}
