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
    void sendTemperaturaCircuito(float floatTemperature, unsigned long epoch);
    void sendFechaCircuitoInicializado(unsigned long epoch);    
    void sendConexionWifi();
    void sendLuzDiurnaEncendida(unsigned long epoch);
    void sendLuzDiurnaApagada(unsigned long epoch);
    void sendLuzDeLunaEncendida(unsigned long epoch);
    void sendLuzDeLunaApagada(unsigned long epoch);
    void sendCalentadorEncendido(unsigned long epoch);
    void sendCalentadorApagado(unsigned long epoch);
    void sendRelojAjustado(unsigned long epoch);
};
#endif
