#ifndef COMMON_H
#define COMMON_H

#include "Arduino.h"

#include <String>

struct NodeInfo {
  int id;
  String name;
  int temp;
  int humidity;
};

struct Nodes {
  NodeInfo nodes[4];
};

#endif
