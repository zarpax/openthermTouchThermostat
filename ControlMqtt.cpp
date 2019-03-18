#include "ControlMqtt.h"

ControlMqtt::ControlMqtt() {
  mqttClient = new PubSubClient(espClient);
  
  IPAddress mqttServer(192, 168, 2, 8);
  
  mqttClient->setServer(mqttServer, 1883);
}

void ControlMqtt::reconnect() {
    unsigned int retries = 3; //onfiguracionSistema::TRIES_RECONNET;
    unsigned int intento = 0;
      
  while ((!mqttClient->connected()) && (intento < retries)) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "AcuarioClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient->connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //mqttClient.publish("outTopic", "hello world");
      // ... and resubscribe
      //mqttClient.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient->state());
      Serial.println(" try again in 1 seconds");
      // Wait 1 seconds before retrying
      delay(1000);
      intento++;
    }
  }
}

void ControlMqtt::sendMqtt(const char* topic, const char* mensaje) {
    if (!mqttClient->connected()) {
        reconnect();
    }
      
    mqttClient->loop(); 
    
    mqttClient->publish(topic, mensaje);    
}

void ControlMqtt::sendTemperature(float floatTemperature, unsigned long epoch) {
    String strTemp = String(floatTemperature, 2);  
    char temperatura[5] = "";  
    strTemp.toCharArray(temperatura, 5);    

      char charEpoc[50];
      ltoa(epoch, charEpoc, 10);    
      
      char mensaje[55];
      sprintf(mensaje,"%s_%s", charEpoc, temperatura);
      
    this->sendMqtt("casa/acuario/temperatura", mensaje);
}

void ControlMqtt::sendTemperaturaCircuito(float floatTemperature, unsigned long epoch) {
    String strTemp = String(floatTemperature, 2);  
    char temperatura[5] = "";  
    strTemp.toCharArray(temperatura, 5);    

      char charEpoc[50];
      ltoa(epoch, charEpoc, 10);    
      
      char mensaje[55];
      sprintf(mensaje,"%s_%s", charEpoc, temperatura);
      
    this->sendMqtt("casa/acuario/temperaturaCircuito", mensaje);
}

void ControlMqtt::sendFechaCircuitoInicializado(unsigned long epoch) {
    char charEpoc[50];
    ltoa(epoch, charEpoc, 10);      
    this->sendMqtt("casa/acuario/inicioSistema",charEpoc);
}

void ControlMqtt::sendConexionWifi() {
  this->sendMqtt("casa/acuario/conexionWifi","");
}

void ControlMqtt::sendLuzDiurnaEncendida(unsigned long epoch) {
    char charEpoc[50];
    ltoa(epoch, charEpoc, 10);      
  this->sendMqtt("casa/acuario/luzDiurnaEncendida",charEpoc);
}

void ControlMqtt::sendLuzDiurnaApagada(unsigned long epoch) {
    char charEpoc[50];
    ltoa(epoch, charEpoc, 10);      
  this->sendMqtt("casa/acuario/luzDiurnaApagada",charEpoc);
}

void ControlMqtt::sendLuzDeLunaEncendida(unsigned long epoch) {
    char charEpoc[50];
    ltoa(epoch, charEpoc, 10);      
  this->sendMqtt("casa/acuario/luzDeLunaEncendida",charEpoc);
}

void ControlMqtt::sendLuzDeLunaApagada(unsigned long epoch) {
    char charEpoc[50];
    ltoa(epoch, charEpoc, 10);      
  this->sendMqtt("casa/acuario/luzDeLunaApagada",charEpoc);
}

void ControlMqtt::sendCalentadorEncendido(unsigned long epoch) {
    char charEpoc[50];
    ltoa(epoch, charEpoc, 10);      
  this->sendMqtt("casa/acuario/calentadorEncendido",charEpoc);
}

void ControlMqtt::sendCalentadorApagado(unsigned long epoch) {
    char charEpoc[50];
    ltoa(epoch, charEpoc, 10);      
  this->sendMqtt("casa/acuario/calentadorApagado",charEpoc);
}

void ControlMqtt::sendRelojAjustado(unsigned long epoch) {
    char charEpoc[50];
    ltoa(epoch, charEpoc, 10);      
  this->sendMqtt("casa/acuario/relojAjustado",charEpoc);
}
