#pragma once
#include "Arduino.h"
#include <Wire.h>

class Wind {
 public:
  Wind(int addr) : addr_(addr) {}
  void init() { Wire.begin(); }
  int get_reading() {
    // step 1: instruct sensor to read echoes
    Wire.beginTransmission(addr_);
    // the address specified in the datasheet is 66 (0x42)
    // but i2c adressing uses the high 7 bits so it's 33
    Wire.write('A'); // Command to measure angle.
    Wire.endTransmission();
    // Wait for readings.
    delay(10); // Datasheet suggests at least 6000us.
    Wire.requestFrom(addr_, 2);
    if (2 <= Wire.available()) {
      reading = Wire.read();  // receive high byte (overwrites previous reading)
      reading = reading << 8; // shift high byte to be high 8 bits
      reading += Wire.read(); // receive low byte as lower 8 bits
      reading /= 10;
    } else {
      reading = -1;
    }
    return reading;
  }
 private:
  const int addr_;
  int reading = -1;
};
