#ifndef GWIFI_H
#define GWIFI_H
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

class ControlWifi {
  private:
    //const int NTP_PACKET_SIZE = 48;
    const unsigned int localPort = 2390;
    
    //const double timezone = 2.0; // Verano
    const double timezone = 1.0; // Invierno

    WiFiClient client;
    byte eRcv();
    
  public:
    ControlWifi();
    void connectToWifi();
    
    boolean isConnected();
    unsigned long getTimeFromTimeServer();
};
#endif
