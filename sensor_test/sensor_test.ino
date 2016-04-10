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

int flw_ctl_pin = 2;
PPM pins(2);
SerialFlow serial(9600, flw_ctl_pin);
Wind wind(compassAddress);
Servo winch, rudder;
int winch_pin = 10, rudder_pin = 9;
float winch_avg = 90, rudder_avg = 90;
int winch_channel = 3, rudder_channel = 4; // PPM channels. TODO: Figure out which channels to use.
int rudder_middle_micro = 1200;
int rudder_range_micro = 600; // ie, rudder_middle_micro +/- rudder_range_micro/2
int winch_middle_micro = 1450;
int winch_range_micro = 500;
int upwind_reading = 90;

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
  winch.writeMicroseconds(1200); // Going out is the least destructive option.
  rudder.attach(rudder_pin, 800, 1600);

  Serial.println("Hello, World!");
  /* Initialise the IMU */
  //if(!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
  //  Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  //}
  //Serial.println("BNO begun, World!");
  //bno.setExtCrystalUse(true);
  // Compass sensor setup.
  // Wire.begin is no longer necessary, as the BNO055 will begin it
  // automatically.
  Wire.begin();       // join i2c bus (address optional for master)
  if (1 == 1) {
    Serial.println("Doing compass calibration\n.");
    // Do compass calibration.
    Wire.beginTransmission(compassAddress);
    Wire.write('C');
    Wire.endTransmission();
    for (int i = 0; i < 25; ++i) {
    Serial.println("Doing compass calibration\n.");
      Serial.println(i);
      delay(1000);
    }
    Wire.beginTransmission(compassAddress);
    Wire.write('E');
    Wire.endTransmission();
  }
  // When the calibration is done, the user should set the
  // wind vane to its downwind position.
  upwind_reading = wind.get_reading() - 180;
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

  if (false) {
    serial.print("PPM: ");
    serial.print(pins.getChannel(0));
    serial.print("\t");
    serial.print(pins.getChannel(1));
    serial.print("\t");
    serial.print(pins.getChannel(2));
    serial.print("\t");
    serial.print(pins.getChannel(3));
    serial.print("\t");
    serial.print(pins.getChannel(4));
    serial.print("\t");
    serial.print(pins.getChannel(5));
    serial.print('\n');
  }

  if (auto_winch == 1) {
    if (reading != -1) {
      // Basically, we just say that if we are going into the wind, we pull the
      // sails all the way in; if the wind is right behind us we go all out, and
      // we vary automatically in between.
      // We now translate it so that 0=upwind, +180 = downwind.
      float abs_reading = angle_diff_deg(reading, upwind_reading);
      abs_reading = -abs(abs_reading);
      // We now want to map the apparent wind to some choice of winch
      // goal. The winch should be all the way in when going near to
      // upwind and all the way out when going straight downwind.
      int write_val = (abs_reading / 180 + 0.5) * winch_range_micro +
                      winch_middle_micro;
      serial.print(" Winch: ");
      serial.print(write_val);
      serial.print('\n');
      winch.writeMicroseconds(write_val);
    }
  } else {
    int reading = pins.getChannel(winch_channel);
    if (reading >= 0 && reading < 195) {
      if (reading > 180) reading = 180;
      reading = constrain(reading, winch_avg - 8, winch_avg + 8);
      winch_avg = (float)winch_avg * .7 + (float)reading * .3;
      int write_val = (winch_avg - 90.) / 180. * winch_range_micro + winch_middle_micro;
      winch.writeMicroseconds(write_val);
      serial.print("Winch: ");
      serial.print(write_val);
      serial.print('\n');
    }
    //winch.write(pins.getChannel(winch_channel));
    //winch.write(90);
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
  serial.print(roll);
  serial.print("\t");
  serial.print(yaw);
  serial.print('\n');

  if (auto_rudder == 1) {
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
    //rudder.write(kPrudder * heading_diff + 90);
  } else {
    int reading = pins.getChannel(rudder_channel);
    if (reading >= 0 && reading < 195) {
      if (reading > 180) reading = 180;
      reading = constrain(reading, rudder_avg - 8, rudder_avg + 8);
      rudder_avg = (float)rudder_avg * .7 + (float)reading * .3;
      int write_val = (rudder_avg - 90.) / 180. * rudder_range_micro + rudder_middle_micro;
      rudder.writeMicroseconds(write_val);
      //Serial.println(write_val);
      //rudder.write(90);
    }
  }

  delay(20);

}
