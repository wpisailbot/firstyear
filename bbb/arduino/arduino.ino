#include "Arduino.h"
#include "PPM.h"
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "Wind.h"

int wind_port = 0;
int winch_pot = 1, rudder_pot = 2;

PPM pins(2);
Wind wind(wind_port);

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

void setup() {
  Serial.begin(9600);
  wind.init();

  Serial.println("Hello, World!");
  /* Initialise the IMU */
  if(!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  }
  Serial.println("BNO begun, World!");
  bno.setExtCrystalUse(true);
  uint8_t syscal=0, gyrocal=0, accelcal=0, magcal=0;
  char buf[32];
  do {
    bno.getCalibration(&syscal, &gyrocal, &accelcal, &magcal);
    snprintf(buf, 32, "IMU cal: %d %d %d %d\n", syscal, gyrocal, accelcal, magcal);
    Serial.print(buf);
    // Do while any one of the values is zero.
  } while (!(syscal && gyrocal && accelcal && magcal));

  // The wind sensor should be pointed downwind; this will calibrate it as such.
  wind.calibrate_downwind();

  Serial.println("Hello World");
}

void loop() {
  Serial.print("PPM");
  for (int i = 0; i < 6; ++i) {
    Serial.print(" ");
    Serial.print(pins.getChannel(i));
  }
  Serial.println();

  // Do wind sensor stuff.
  int reading = wind.get_reading();
  Serial.print("Wind ");
  Serial.print(reading); // print the wind reading
  Serial.println();

  // Get heel and heading information.
  imu::Quaternion quat = bno.getQuat();
  imu::Vector<3> euler = quat.toEuler();
  // euler[0] and euler[2] range from [-pi, +pi].
  // euler[1] is [-pi/2, +pi/2]
  double roll = euler[2];
  double pitch = euler[1];
  double yaw = euler[0];
  Serial.print("roll ");
  Serial.println(roll);
  Serial.print("yaw ");
  Serial.println(yaw);

  // Print out winch and rudder pots.
  Serial.print("Wpot ");
  Serial.println(analogRead(winch_pot));
  Serial.print("Rpot ");
  Serial.println(analogRead(rudder_pot));
}
