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
                      
WiFiClient client;
HTTPClient http;

ControlOT* controlOT;
ControlWifi* controlWifi;
ControlMqtt* mqtt;
ControlTouchScreen* ts;

void setup() {
    Serial.begin(115200);
    //Serial.setDebugOutput(true);
    
    controlOT = new ControlOT();
    controlWifi = new ControlWifi();
    controlWifi->connectToWifi();
    mqtt = new ControlMqtt();
    ts = new ControlTouchScreen(controlOT, palette);
}

unsigned long previousMillis = 0;       
unsigned long interval = 1500;
boolean areEquals = true;

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    String payload = getTempsFromServer();
    areEquals = controlOT->processServerResponse(payload);
    yield();
  }

  if (!areEquals) {
    Serial.println("Actualizando pantalla......");
    ts->paintScreen();
    ESP.wdtFeed();
    areEquals = true;  
  }  
  ESP.wdtFeed();
}

String getTempsFromServer() {
    http.begin(client, "192.168.2.8", 1880, "/estado");
    yield();
    int httpCode = http.GET();
    yield();

    String payload;
    
    if (httpCode > 0) {
      payload = http.getString();
      http.end();   //Close connection  
      yield();
      ESP.wdtFeed();
      return payload;
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      http.end();
      return "";
    }
}
