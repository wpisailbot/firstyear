#pragma once

#include "Arduino.h"
#include <Servo.h>

class Winch {
 public:
  Winch(int winch_port, int pot_port)
      : winch_port_(winch_port), pot_(pot_port) {}
  void init() { winch_.attach(winch_port_, 1200, 1700); }

 private:
  // each entry in this array corresponds to a pot angle
  // for the boom from 0 (all the way in) to 90 (all the way out).
  // It scales linearly, so if there are 10 elements in the array
  // then they will correspond to angles of {0, 10, 20, 30, 40, 50, 60, 70, 80,
  // 90}. The values in the array are in winch servo "percentages" (0-100).
  int pot_to_winch[11] = {0, 11, 22, 33, 44, 56, 67, 78, 89, 100};
  Servo winch_;
  int pot_;
  int winch_port_;
};
