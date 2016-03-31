#pragma once
#include "Arduino.h"
#include <math.h>

class Accel {
 public:
  Accel(int x_channel, int y_channel, int z_channel)
      : channels_{x_channel, y_channel, z_channel} {}
  float Heel() {
    // TODO: Figure out which axes to do atan of.
    return atan2(GetY(), GetX());
  }
  int GetX() { return analogRead(channels_[0]); }
  int GetY() { return analogRead(channels_[1]); }
  int GetZ() { return analogRead(channels_[2]); }

 private:
  const int channels_[3];
};
