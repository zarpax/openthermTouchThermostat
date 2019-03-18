#include "ControlMqtt.h"
#include "ControlTouchScreen.h"

ControlMqtt* mqtt;
ControlTouchScreen* ts;

bool done = false;

void setup() {
    mqtt = new ControlMqtt();
    ts = new ControlTouchScreen();
}

void loop() {
  if (!done) {
    ts->paintScreen();
    done = true;
  }
}
