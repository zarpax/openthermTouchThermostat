#ifndef COMMON_H
#define COMMON_H

#include "Arduino.h"
#include <simpleDSTadjust.h>
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

const String WDAY_NAMES[] = {"DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

#define UTC_OFFSET +1
const struct dstRule StartRule = {"CEST", Last, Sun, Mar, 2, 3600}; // Central European Summer Time = UTC/GMT +2 hours
const struct dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0};       // Central European Time = UTC/GMT +1 hour
#define NTP_SERVERS "0.ch.pool.ntp.org", "1.ch.pool.ntp.org", "2.ch.pool.ntp.org"
// Change for 12 Hour/ 24 hour style clock
const bool IS_STYLE_12HR = false;

#endif
