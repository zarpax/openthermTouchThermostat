#include "ControlMqtt.h"
#include "ControlWifi.h"
#include "ControlTouchScreen.h"
#include "ControlOT.h"
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

uint16_t palette[] = {ILI9341_BLACK, // 0
                      ILI9341_WHITE, // 1
                      ILI9341_NAVY, // 2
                      ILI9341_DARKCYAN, // 3
                      ILI9341_DARKGREEN, // 4
                      ILI9341_MAROON, // 5
                      ILI9341_PURPLE, // 6
                      ILI9341_OLIVE, // 7
                      ILI9341_LIGHTGREY, // 8
                      ILI9341_DARKGREY, // 9
                      ILI9341_BLUE, // 10
                      ILI9341_GREEN, // 11
                      ILI9341_CYAN, // 12
                      ILI9341_RED, // 13
                      ILI9341_MAGENTA, // 14
                      ILI9341_YELLOW}; // 15  
                      
HTTPClient http;

float sp = 23, //set point
pv = 0, //current temperature
pv_last = 0, //prior temperature
ierr = 0, //integral error
dt = 0, //time between measurements
op = 0; //PID controller output
unsigned long ts = 0, new_ts = 0; //timestamp
bool enableCentralHeating = true;
bool enableHotWater = true;
bool enableCooling = true;

bool isLastHotWaterEnabled = false;
bool isLastFlameOn = false;
float lastTemperature = 0;

#define OPENTHERM_IN D0
#define OPENTHERM_OUT D8
const int inPin = 2; // ESP RX
const int outPin = 1; // ESP TX
//OpenTherm ot(inPin, outPin);
OpenTherm ot(OPENTHERM_IN, OPENTHERM_OUT);

ControlOT* controlOT;
ControlWifi* controlWifi;
ControlMqtt* mqtt;
ControlTouchScreen* touchScreen;

void setup() {
    Serial.begin(115200);
    //Serial.setDebugOutput(true);

    http.setTimeout(2000);
    ot.begin(handleInterrupt);    
    controlOT = new ControlOT();
    controlWifi = new ControlWifi();
    controlWifi->connectToWifi();
    mqtt = new ControlMqtt();
    touchScreen = new ControlTouchScreen(controlOT, palette);
    updateTime();
}

unsigned long previousMillis = -1;       
unsigned long interval = 5000; // 1 min. 60000
boolean areEquals = true;

unsigned long lastUpdateTime = millis();
void updateTime() {
  configTime(UTC_OFFSET * 3600, 0, NTP_SERVERS);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdateTime >=3600) {
    updateTime();
    lastUpdateTime = millis();
  }
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    String payload = getTempsFromServer();
    areEquals = controlOT->processServerResponse(payload);
    sendMqttOpenTherm();
    yield();
  }
  
  touchScreen->fillBuffer();
  touchScreen->paintScreen(controlWifi->getWifiQuality(), lastTemperature, isLastHotWaterEnabled, isLastFlameOn);
  ESP.wdtFeed();
  yield();
  touchScreen->drawTime();
  ESP.wdtFeed();
  yield();
  touchScreen->commit();
  
  ESP.wdtFeed();
}

String getTempsFromServer() {
    http.begin("192.168.2.8", 1880, "/estado");
    int httpCode = http.GET();
    yield();

    String payload = http.getString();
    Serial.println("Server payload:" + payload);
    if (httpCode > 0) {
      //payload = http.getString();
      http.end();   //Close connection  
      yield();
      ESP.wdtFeed();
      Serial.println("Read OK");
      return payload;
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      http.end();
      return "";
    }
}

void handleInterrupt() {
  ot.handleInterrupt();
}

float pid(float sp, float pv, float pv_last, float& ierr, float dt) {
  float Kc = 10.0; // K / %Heater
  float tauI = 50.0; // sec
  float tauD = 1.0;  // sec
  // PID coefficients
  float KP = Kc;
  float KI = Kc / tauI;
  float KD = Kc*tauD; 
  // upper and lower bounds on heater level
  float ophi = 100;
  float oplo = 0;
  // calculate the error
  float error = sp - pv;
  // calculate the integral error
  ierr = ierr + KI * error * dt;  
  // calculate the measurement derivative
  float dpv = (pv - pv_last) / dt;
  // calculate the PID output
  float P = KP * error; //proportional contribution
  float I = ierr; //integral contribution
  float D = -KD * dpv; //derivative contribution
  float op = P + I + D;
  // implement anti-reset windup
  if ((op < oplo) || (op > ophi)) {
    I = I - KI * error * dt;
    // clip output
    op = max(oplo, min(ophi, op));
  }
  ierr = I; 
  //Serial.println("sp="+String(sp) + " pv=" + String(pv) + " dt=" + String(dt) + " op=" + String(op) + " P=" + String(P) + " I=" + String(I) + " D=" + String(D));
  return op;
}

void sendMqttOpenTherm() {
  Serial.println("sendMqttOpenTherm");
  unsigned long response = ot.setBoilerStatus(enableCentralHeating, enableHotWater, enableCooling);
  
  OpenThermResponseStatus responseStatus = ot.getLastResponseStatus();
  if (responseStatus != OpenThermResponseStatus::SUCCESS) {
    Serial.println("Error: Invalid boiler response " + String(response, HEX));
  }     
  
  float temperature = ot.getTemperature(response);
  bool isHotWaterEnabled = ot.isHotWaterEnabled(response);
  bool isFlameOn = ot.isFlameOn(response);

  if (temperature != lastTemperature) {
    Serial.println("Setting temperature to " + String(temperature));
    // Send mqtt
    lastTemperature = temperature;
  }

  if (isHotWaterEnabled != isLastHotWaterEnabled) {
    Serial.println("Setting isHotWaterEnabled to " + String(isHotWaterEnabled? "On" : "Off"));
    // Send mqtt
    isLastHotWaterEnabled = isHotWaterEnabled;
  }

  if (isFlameOn != isLastFlameOn) {
    Serial.println("Setting isFlameOn to " + String(isFlameOn? "On" : "Off"));
    // Send mqtt
    isLastFlameOn = isFlameOn;
  }

  Serial.println("end sendMqttOpenTherm");
}

void setBoilerTemp() {
    unsigned long response = ot.setBoilerStatus(enableCentralHeating, enableHotWater, enableCooling);
    OpenThermResponseStatus responseStatus = ot.getLastResponseStatus();
    if (responseStatus != OpenThermResponseStatus::SUCCESS) {
      Serial.println("Error: Invalid boiler response " + String(response, HEX));
    }   

    pv = controlOT->getMediumTemp();
    dt = (new_ts - ts) / 1000.0;
    ts = new_ts;
    if (responseStatus == OpenThermResponseStatus::SUCCESS) {
      op = pid(sp, pv, pv_last, ierr, dt);
      ot.setBoilerTemperature(op);
    }
    pv_last = pv;
}
