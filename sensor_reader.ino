#include <FastLED.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

#define LED_AMT 1
#define LED_PIN D7

int ledNum = 0;
int R = 0;
int G = 0;
int B = 0;

long pastTime;
int timeDelay = 1000;

CRGB leds[LED_AMT];

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(5);
    delay(500);
    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
      delay(50);
    }
    FastLED.addLeds<WS2812, LED_PIN, RGB>(leds, LED_AMT);
}

void parseString(String inputMessage) {
  if(inputMessage.substring(0,2) == ("R:")){ // for "Rainbow" RGB light commands
    int CMD1POS = inputMessage.indexOf("1:");
    int CMD2POS = inputMessage.indexOf("2:");
    int CMD3POS = inputMessage.indexOf("3:");
    int CMD4POS = inputMessage.indexOf("4:");
    int ENDPOS = inputMessage.indexOf("E;");
    
    ledNum = inputMessage.substring(CMD1POS + 2,CMD2POS).toInt();
    G = inputMessage.substring(CMD2POS + 2,CMD3POS).toInt();
    R = inputMessage.substring(CMD3POS + 2,CMD4POS).toInt();
    B = inputMessage.substring(CMD4POS + 2,ENDPOS).toInt();
  }
}

void mpuRead() {
  Vector accel = mpu.readNormalizeAccel();
  Vector gyro = mpu.readNormalizeGyro();  
  String mpuData = "M:1;" + String(accel.XAxis) + ";2:" + String(accel.YAxis) + ";3:" + String(accel.ZAxis) + ";4:" + String(gyro.XAxis) + ";5:" + String(gyro.YAxis) + ";6:" + String(gyro.ZAxis) + ";E;";
  Serial.println(mpuData);
}

void loop() {
  if(Serial.available() > 0) {
    parseString(Serial.readString());
  }
  leds[ledNum] = CRGB(R,G,B);
  FastLED.show();
  if(millis() > pastTime + timeDelay) {
    mpuRead();
    pastTime = millis();
  }
}
