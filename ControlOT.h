#ifndef COT_H
#define COT_H

#include "common.h"
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <OpenTherm.h>

class ControlOT {

  private:
    OpenTherm ot;
    Nodes nodes;
    void updateTemps();
    void createNodesInfo();
    boolean parseServerResponse(String jsonResponse);
    boolean areSameNodeInfo(NodeInfo n1, NodeInfo n2);
    boolean nodesInfoHaveChanged(NodeInfo n);
    boolean copyNewValues(Nodes newNodes);
    void printNodes(Nodes nodesPrint);
    NodeInfo createNodeInfo(int id, String name, int temp, int humidity);
        
  public: 
    ControlOT();
    void handleInterrupt();
    boolean getTempsFromServer(WiFiClient client, HTTPClient http);
    boolean processServerResponse(String payload);
    Nodes getTemps();    
    float getMediumTemp();
};
#endif
