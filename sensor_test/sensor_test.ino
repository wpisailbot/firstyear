#include "Arduino.h"
#include "PPM.h"
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "SerialFlow.h"
#include "util.h"
#include "Wind.h"

int compassAddress = 0x80; // From datasheet compass address is 0x42
// shift the address 1 bit right, the Wire library only needs the 7
// most significant bits for the address
int reading = 0;

int flw_ctl_pin = 5;
PPM pins(2);
SerialFlow serial(9600, flw_ctl_pin);
Wind wind(compassAddress);
Servo winch, rudder;
int winch_pin, rudder_pin;
int winch_channel = 1, rudder_channel = 2; // PPM channels. TODO: Figure out which channels to use.

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

int auto_winch = 0;
int auto_rudder = 0;

void setup() {
  serial.init();
  wind.init();
  // Roughly speaking, 1700 is all the way in for the winch.
  // 1200 is all the way out (give or take).
  winch.attach(winch_pin, 1200, 1700);
  rudder.attach(rudder_pin, 500, 2500);

  /* Initialise the IMU */
  if(!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  bno.setExtCrystalUse(true);

  // Compass sensor setup.
  // Wire.begin is no longer necessary, as the BNO055 will begin it
  // automatically.
  //Wire.begin();       // join i2c bus (address optional for master)
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

  if (auto_winch) {
    if (reading != -1) {
      // Basically, we just say that if we are going into the wind, we pull the
      // sails all the way in; if the wind is right behind us we go all out, and
      // we vary automatically in between.
      // We will assume that the reading is such that 180=into the wind,
      // 0/360 means we are going downwind.
      // We now translate it so that 0=upwind, +180 = downwind.
      int abs_reading = abs(reading - 180);
      // We now want to map the apparent wind to some choice of winch
      // goal. The winch should be all the way in when going near to
      // upwind and all the way out when going straight downwind.
      // Servo.write values range from 0 - 180.
      winch.write(abs_reading);
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

  // Get heel and heading information.
  imu::Quaternion quat = bno.getQuat();
  imu::Vector<3> euler = quat.toEuler();
  // euler[0] and euler[2] range from [-pi, +pi].
  // euler[1] is [-pi/2, +pi/2]
  double roll = euler[0];
  double pitch = euler[1];
  double yaw = euler[2];

  if (auto_rudder) {
    double goal_heading = 0;
    double heading_diff = angle_diff(goal_heading, yaw);
    // Logic so that our goal heading doesn't send us into the wind.
    // Remember, reading = 180 is into the wind.
    double wind_reading = (double)(reading - 180) * PI / 180.;
    double heading_wind = angle_sum(heading_diff, wind_reading);
    double max_close_haul = PI / 6;
    double max_head = angle_sum(wind_reading, max_close_haul);
    double min_head = angle_diff(wind_reading, max_close_haul);

    if (heading_diff > min_head && heading_wind < 0)
      heading_diff = -max_close_haul;
    else if (heading_diff < max_close_haul && heading_wind > 0)
      heading_diff = max_close_haul;
    // Now, calculate rudder angle
    double kPrudder = 100.0;
    rudder.write(kPrudder * heading_diff + 90);
  } else {
    rudder.write(pins.getChannel(rudder_channel));
  }

}
