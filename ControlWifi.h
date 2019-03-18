#ifndef GWIFI_H
#define GWIFI_H
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "GestionMqtt.h"

class GestionWifi {
  private:
    //const int NTP_PACKET_SIZE = 48;
    const unsigned int localPort = 2390;
    
    //const double timezone = 2.0; // Verano
    const double timezone = 1.0; // Invierno

    GestionMqtt *gMqtt;
    ESP8266WiFiClass *wifi;
    WiFiClient client;
    byte eRcv();
    
  public:
    GestionWifi(GestionMqtt *mqtt);
    void connectToWifi();
    
    boolean isConnected();
    unsigned long getTimeFromTimeServerOLD();
    unsigned long getTimeFromTimeServer();
    byte sendEmail(String strEmailSubject, String strEmailContent);    
};
#endif
