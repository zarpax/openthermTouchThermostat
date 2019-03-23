#include "ControlOT.h"

ControlOT::ControlOT() {
    createNodesInfo();
}

void ControlOT::createNodesInfo() {
  NodeInfo nodeLivingRoom = createNodeInfo(4, "Salón", 0, 0); 
  NodeInfo nodeKidsRoom = createNodeInfo(11, "Nenes", 0, 0); 
  NodeInfo nodeExternal = createNodeInfo(1, "Balcón", 0, 0);
  NodeInfo nodeWorkingRoom = createNodeInfo(5, "Estudio", 0, 0);
  
  nodes.nodes[0] = nodeLivingRoom;
  nodes.nodes[1] = nodeKidsRoom;
  nodes.nodes[2] = nodeExternal;
  nodes.nodes[3] = nodeWorkingRoom;
}

Nodes ControlOT::getTemps() {
  return nodes;
}

boolean ControlOT::parseServerResponse(String jsonResponse) {
  DynamicJsonDocument doc(2048);
  Nodes newNodes;
  boolean areEquals = true;
  DeserializationError error = deserializeJson(doc, jsonResponse);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return areEquals;
  }  
  yield();
  
  int node = 0;
  
  while(node < 4) {
    
      int temp = doc["nodes"][node]["T"];
      int humidity = doc["nodes"][node]["H"];
          
    if (strcmp(doc["nodes"][node]["nodo"], "Nenes") == 0) {
      //Serial.println("Encontrado nodo Nenes");
      areEquals = (areEquals && (nodes.nodes[1].temp == temp) && (nodes.nodes[1].humidity == humidity)); 
      nodes.nodes[1].temp = temp;
      nodes.nodes[1].humidity = humidity;
    } else if (strcmp(doc["nodes"][node]["nodo"], "Balcón") == 0) {
      //Serial.println("Encontrado nodo Balcón");
      areEquals = (areEquals && (nodes.nodes[2].temp == temp) && (nodes.nodes[2].humidity == humidity)); 
      nodes.nodes[2].temp = temp;
      nodes.nodes[2].humidity = humidity;
    } else if (strcmp(doc["nodes"][node]["nodo"], "Estudio") == 0) {
      //Serial.println("Encontrado nodo Estudio");
      areEquals = (areEquals && (nodes.nodes[3].temp == temp) && (nodes.nodes[3].humidity == humidity)); 
      //Serial.print("Post Are Equals ");Serial.println(areEquals);
      nodes.nodes[3].temp = temp;
      nodes.nodes[3].humidity = humidity;
    } else if (strcmp(doc["nodes"][node]["nodo"], "Salón") == 0) {
      //Serial.println("Encontrado nodo Salón");
      areEquals = (areEquals && (nodes.nodes[0].temp == temp) && (nodes.nodes[0].humidity == humidity)); 
      nodes.nodes[0].temp = temp;
      nodes.nodes[0].humidity = humidity;  
    }    

    yield();
    node++;
  }

  return areEquals;
}

boolean ControlOT::getTempsFromServer(WiFiClient client, HTTPClient http) {
    //HTTPClient http;
    //Serial.println("Haciendo peticion");
    //WiFiClient client;
    http.begin(client, "192.168.2.8", 1880, "/estado");
    yield();
    //Serial.println("Peticion hecha");
    int httpCode = http.GET();
    yield();
    //Serial.println("httpCode " + String(httpCode));

    String payload;
    
    if (httpCode > 0) {
      payload = http.getString();
      //Serial.println(payload);      
      http.end();   //Close connection  
      yield();
      
      Serial.println("Parsing response....");
      boolean changes = parseServerResponse(payload);
      yield();
      ESP.wdtFeed();
      Serial.print("All done..... Changes=");
      Serial.println(changes);
      return changes;
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      http.end();
      return false;
    }
}

boolean ControlOT::processServerResponse(String payload) {    
      //Serial.println("Parsing response....");
      boolean changes = parseServerResponse(payload);
      yield();
      ESP.wdtFeed();
      //Serial.print("All done..... Changes=");
      //Serial.println(changes);
      return changes;
}

boolean ControlOT::areSameNodeInfo(NodeInfo n1, NodeInfo n2) {
  if (n1.id != n2.id) {
    return false;
  }
  
  if (n1.name != n2.name) {
    return false;
  }

  return true;
}

boolean ControlOT::nodesInfoHaveChanged(NodeInfo n) {
  boolean hasChanges = false;
  for(int i = 0; i < 4; i++) {
    if ((areSameNodeInfo(nodes.nodes[i], n)) && ((nodes.nodes[i].temp != n.temp) || (nodes.nodes[i].humidity != n.humidity))) {
        hasChanges = true;
        break;
    }
  }

  return hasChanges;
}

boolean ControlOT::copyNewValues(Nodes newNodes) {
  int node = 0;
  boolean areEquals = true;

  /*Serial.println("Nuevos valores");
  printNodes(newNodes);
  Serial.println("Valores almacenados");
  printNodes(nodes);*/
  
  while(node < 4) {
    //areEquals = areEquals && nodesInfoHaveChanged(newNodes.nodes[node]);
    yield();
    
    if (newNodes.nodes[node].name == "Nenes") {
      Serial.println("Comparando nodo Nenes");     
      areEquals = areEquals && ((nodes.nodes[1].temp != newNodes.nodes[node].temp) || (nodes.nodes[1].humidity != newNodes.nodes[node].humidity)); 
      nodes.nodes[1].temp = newNodes.nodes[node].temp;
      nodes.nodes[1].humidity = newNodes.nodes[node].humidity;
    } else if (newNodes.nodes[node].name == "Balcón") {
      Serial.println("Comparando nodo Balcón");
      areEquals = areEquals && ((nodes.nodes[2].temp != newNodes.nodes[node].temp) || (nodes.nodes[2].humidity != newNodes.nodes[node].humidity)); 
      nodes.nodes[2].temp = newNodes.nodes[node].temp;
      nodes.nodes[2].humidity = newNodes.nodes[node].humidity;
    } else if (newNodes.nodes[node].name == "Estudio") {
      Serial.println("Comparando nodo Estudio");
      areEquals = areEquals && ((nodes.nodes[3].temp != newNodes.nodes[node].temp) || (nodes.nodes[3].humidity != newNodes.nodes[node].humidity)); 
      nodes.nodes[3].temp = newNodes.nodes[node].temp;
      nodes.nodes[3].humidity = newNodes.nodes[node].humidity;
    } else if (newNodes.nodes[node].name == "Salón") {
      Serial.println("Comparando nodo Salón");
      areEquals = areEquals && ((nodes.nodes[0].temp != newNodes.nodes[node].temp) || (nodes.nodes[0].humidity != newNodes.nodes[node].humidity)); 
      nodes.nodes[0].temp = newNodes.nodes[node].temp;
      nodes.nodes[0].humidity = newNodes.nodes[node].humidity;
    }    
    ESP.wdtFeed();
    yield();
    node++;
  }

  return areEquals;
}

void ControlOT::printNodes(Nodes nodesPrint) {
  for(int i = 0; i < 4; i++) {
    Serial.print("Id ");Serial.print(nodesPrint.nodes[i].id);
    Serial.print(" Nodo " + nodesPrint.nodes[i].name);
    Serial.print(" Temp ");Serial.print(nodesPrint.nodes[i].temp);
    Serial.print(" Hume ");Serial.println(nodesPrint.nodes[i].humidity);
  }
}

NodeInfo ControlOT::createNodeInfo(int id, String name, int temp, int humidity) {
  NodeInfo node;
  node.id = id;
  node.name = name;
  node.temp = temp;
  node.humidity = humidity;

  return node;
}

float ControlOT::getMediumTemp() {
  return (nodes.nodes[0].temp + nodes.nodes[3].temp) / 2;
}
