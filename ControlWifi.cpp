#include "ControlWifi.h"

ControlWifi::ControlWifi() {  
}

void ControlWifi::connectToWifi() {
   ESP8266WiFiMulti WiFiMulti;
   
  if (WiFiMulti.run() != WL_CONNECTED) {
    unsigned int retries = 15; // Add to settings
    unsigned int intento = 0;

    const char ssid[] = "X";
    const char pass[] = "@Juanan07Sofia06Hugo11Jaime08@";
               
    while ((WiFiMulti.run() != WL_CONNECTED) && (intento < retries))
    { 
      WiFi.mode(WIFI_STA);
      WiFiMulti.addAP(ssid, pass);
      ESP.wdtFeed();      
      yield();
      ESP.wdtFeed();
      //ConfiguracionSistema::myDelay(500); add to settings
      yield();
      ESP.wdtFeed();
      Serial.print(".");
      intento++;
      delay(500);
    }

    //ConfiguracionSistema::myDelay(500); Add to settings
    yield();
    ESP.wdtFeed();
    yield();
    ESP.wdtFeed();
    yield();
    ESP.wdtFeed();

    //Serial.println("WIFI status:" + wifi->status());  
    
    if (WiFiMulti.run() == WL_CONNECTED) {      
      Serial.println("Connected to WIFI");
      Serial.println(WiFi.localIP());
    } 
  } 

  yield();
  ESP.wdtFeed();
}

boolean ControlWifi::isConnected() {
  ESP8266WiFiMulti WiFiMulti;
  boolean status = (WiFiMulti.run() == WL_CONNECTED);
  return status;
}

int8_t ControlWifi::getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if(dbm <= -100) {
      return 0;
  } else if(dbm >= -50) {
      return 100;
  } else {
      return 2 * (dbm + 100);
  }
}
