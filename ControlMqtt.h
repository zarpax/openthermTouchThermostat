#ifndef GMQTT_H
#define GMQTT_H
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class ControlMqtt {
  private:    
    WiFiClient espClient;
    PubSubClient *mqttClient;
    void reconnect();

    void sendMqtt(const char* topic, const char* mensaje);
  public:
    ControlMqtt();
    void sendTemperature(float temperature, unsigned long epoch);
    void sendLuzDiurnaEncendida(unsigned long epoch);
    void sendLuzDiurnaApagada(unsigned long epoch);

};
#endif
