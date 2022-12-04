#include <ESP8266WiFi.h>
#include "PID.h"

PID lPID;
PID rPID;

#define RXBUFFERSIZE 1024
#define STACK_PROTECTOR  512
#define MAX_SRV_CLIENTS 5

#define ENA D4
#define IN1 D2
#define IN2 D3
#define IN3 D6
#define IN4 D7
#define ENB D8

#define EN1 D1
#define EN2 D5
 
int leftSpeed = 0;
int rightSpeed = 0;
int leftCount = 0;
int rightCount = 0;
int pastLeftCount = 0;
int pastRightCount = 0;
int leftRPM = 0;
int rightRPM = 0;
int minSpeed = 30;



long currentTime = 0;
long pastTime = 0;
long timeDelay = 75;

String sbuf = "510510";
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

/*
 * Important things to know:
 * - Uses telnet connection on ESP8266 12F
 * - Runs on 2 TT motors with l298n
 * - Max RPM on decent batteries ~ 210 RPM
 * - 180 RPM = 1/3 m/s
 */
void setup() {
  pinMode(ENA,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENB,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(EN1),encoderPulseL,CHANGE);
  attachInterrupt(digitalPinToInterrupt(EN2),encoderPulseR,CHANGE);
  
  Serial.begin(115200);
  Serial.setTimeout(1);
  initServer();
  
  lPID.setPID(0.9,0.0004,0.0);
  rPID.setPID(0.9,0.0004,0.0);
  lPID.setSetPoint(0);
  rPID.setSetPoint(0);
}

void loop() {
  serverUpdate();
  finalRPM();
  leftSpeed = lPID.calculate(leftRPM);
  Serial.print("leftRPM = ");
  Serial.println(leftRPM);
  rightSpeed = rPID.calculate(rightRPM);
  Serial.print("rightRPM = ");
  Serial.println(rightRPM);
  leftDrive(leftSpeed);
  rightDrive(rightSpeed);
}

void initServer() {
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

ICACHE_RAM_ATTR void encoderPulseL() {
  leftCount += leftSpeed / abs(leftSpeed);
}

ICACHE_RAM_ATTR void encoderPulseR() {
  rightCount += rightSpeed / abs(rightSpeed);
}

double findRPM(double past,double current, double timeDifferential) {
  return((current - past) * (60000 / (timeDifferential * 40)));
}

void finalRPM() {
  currentTime = millis();
  if(currentTime >= pastTime + timeDelay) {
    leftRPM = findRPM(pastLeftCount,leftCount,timeDelay);
    rightRPM = findRPM(pastRightCount,rightCount,timeDelay);
    pastRightCount = rightCount;
    pastLeftCount = leftCount;
    pastTime = currentTime;
  }
}

void leftDrive(int driveSpeed) {
  if(driveSpeed > minSpeed) {
    analogWrite(ENA,abs(driveSpeed * 2.55));
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
  } else if(driveSpeed < -minSpeed) {
    analogWrite(ENA,abs(driveSpeed * 2.55));
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
  } else {
    analogWrite(ENA,0);
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,HIGH);
  }
}

void rightDrive(int driveSpeed) {
  if(driveSpeed > minSpeed) {
    analogWrite(ENB,abs(driveSpeed * 2.55));
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
  } else if(driveSpeed < -minSpeed) {
    analogWrite(ENB,abs(driveSpeed * 2.55));
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
      lPID.setSetPoint((sbuf.substring(0,3).toInt() - 510) / 1.417);
      rPID.setSetPoint((sbuf.substring(3,6).toInt() - 510) / 1.417);
      Serial.println((sbuf.substring(0,3).toInt() - 510) / 1.417);
    }
  }
}
