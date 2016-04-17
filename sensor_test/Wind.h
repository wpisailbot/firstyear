#pragma once
#include "Arduino.h"
#include <Wire.h>
#include <math.h>

class Wind {
 public:
  Wind(int addr) : addr_(addr) {}
  void init() { }
  int get_reading() {
    return analogRead(addr_) * 360. / 1024.;
  }
 private:
  const int addr_;
};
