#pragma once
#include "Arduino.h"
#include <Wire.h>
#include <math.h>
#include "util.h"

class Wind {
 public:
  Wind(int addr) : addr_(addr) {}
  void init() { }
  void calibrate_downwind() {
    upwind_reading = get_reading() - 180;
  }
  int get_reading() {
    int reading =  analogRead(addr_) * 360. / 1024.;
    return angle_diff_deg(reading, upwind_reading);
  }
 private:
  const int addr_;
  int upwind_reading = 0;
};
