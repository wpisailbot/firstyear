#pragma once
#include "Arduino.h"

class SerialFlow {
 /**
  * The SerialFlow class is meant for use with an X-bee, where
  * when you write data to the xbee, it is possible to fill up the
  * buffer.
  * On the X-bee, when there are 17 bytes remaining in the buffer,
  * the CTS pin will go high. Once there are 34 bytes free again,
  * the CTS pin will re-assert low.
  * See https://www.sparkfun.com/datasheets/Wireless/Zigbee/XBee-2.5-Manual.pdf
  * section 2.1.3 (page 12).
  */
 public:
  SerialFlow(int baud, int flow_pin) : flow_pin_(flow_pin), baud_(baud) {}
  void init() {
    Serial.begin(baud_);
    pinMode(flow_pin_, INPUT);
  }
  template <typename T>
  void print(T data) {
    // Note: if data is too long, the cts pin might activate
    // while serial is printing and we will end up ignoring it.
    // The xbee can take up to 34 bytes after the cts pin clears.
    while (!cts()) continue;
    Serial.print(data);
  }
 private:
  const int flow_pin_;
  const int baud_;
  bool cts() { return !digitalRead(flow_pin_); }
};
