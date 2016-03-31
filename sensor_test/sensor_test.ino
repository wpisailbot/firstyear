#include "Arduino.h"
#include "PPM.h"
#include <Wire.h>
#include <Servo.h>
#include "SerialFlow.h"
#include "Accel.h"
#include "Wind.h"

int compassAddress = 0x80; // From datasheet compass address is 0x42
// shift the address 1 bit right, the Wire library only needs the 7
// most significant bits for the address
int reading = 0;

int flw_ctl_pin = 5;
PPM pins(2);
SerialFlow serial(9600, flw_ctl_pin);
Wind wind(compassAddress);
Accel accel(1, 2, 3);
Servo winch, rudder;
int winch_pin, rudder_pin;
int winch_channel = 1, rudder_channel = 2; // PPM channels. TODO: Figure out which channels to use.

int auto_winch = 0;

void setup() {
  serial.init();
  wind.init();
  // Roughly speaking, 1700 is all the way in for the winch.
  // 1200 is all the way out (give or take).
  winch.attach(winch_pin, 1200, 1700);
  rudder.attach(rudder_pin, 500, 2500);

  // Compass sensor setup.
  Wire.begin();       // join i2c bus (address optional for master)
  if (true) {
    // Do compass calibration.
    Wire.beginTransmission(compassAddress);
    Wire.write('C');
    Wire.endTransmission();
    for (int i = 0; i < 25; ++i) {
      Serial.println(i);
      delay(1000);
    }
    Wire.beginTransmission(compassAddress);
    Wire.write('E');
    Wire.endTransmission();
  }
  Serial.println("Hello World");
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(flw_ctl_pin, INPUT);
  digitalWrite(12, HIGH); // Power supply for wind direction sensor
  digitalWrite(13, LOW); // Power supply for wind direction sensor
  digitalWrite(3, HIGH); // Power supply for GPS
  digitalWrite(4, LOW); // Power supply for GPS
}

void print_gps() {
  // Print whenever a GPRMC message arrives.
  char buf[128];
  int ind = 0;
  bool up_index = false;
  if (Serial.available()) {
    while (true) {
      while (!Serial.available()) continue;
      char chr = Serial.read();
      if (chr == '$') {
        up_index = true;
        ind = 0;
      }
      if (ind == 5) {
        if(strncmp(buf, "$GPRMC", 6)) {
          // Strings are different; retry.
          ind = 0;
          up_index = false;
        }
      }
      buf[ind] = char(chr);
      if (up_index) ++ind;
      if (chr == '\n' && !Serial.available()) break;
    }
  }
  if (ind) serial.write(buf, ind);
}

void loop() {
  bool loop_run = true;
  print_gps();

  serial.print("PPM: ");
  serial.print(pins.getChannel(0));
  serial.print(" ");
  serial.print(pins.getChannel(1));
  serial.print(" ");
  serial.print(pins.getChannel(2));
  serial.print(" ");
  serial.print(pins.getChannel(3));
  serial.print(" ");
  serial.print(pins.getChannel(4));
  serial.print(" ");
  serial.print(pins.getChannel(5));
  serial.print('\n');

  rudder.write(pins.getChannel(rudder_channel));

  if (auto_winch) {
    if (reading != -1) {
      // Basically, we just say that if we are going into the wind, we pull the
      // sails all the way in; if the wind is right behind us we go all out, and
      // we vary automatically in between.
      int abs_reading = reading;
      //float point = (abs_reading - min_read) / (max_read - min_read);
      //winch.write(point * 180);
    }
  } else {
    winch.write(pins.getChannel(winch_channel));
  }

  // Do wind sensor stuff.
  reading = wind.get_reading();
  if (reading != -1) {
    serial.print(reading); // print the reading
  } else {
    serial.print("Data not available");
  }
  serial.print('\n');

  serial.print("Heel: ");
  serial.print(accel.Heel());
  serial.print('\n');

  delay(100);

}
