#!/usr/bin/python
from bbio import *
from math import pi, atan2
from serial_flow import SerialFlow
from motors import Rudder, Winch
from gps import GPS
from ard_int import ArduinoInterface
from controls import get_rudder_angle, get_winch_angle
import haversine

flow_pin = None
winch_pwm = PWM1B
rudder_pwm = PWM1A
waypoints = []
def setup():

  # XBee setup
  global xbee_com
  global xbee_ser
  Serial2.begin(9600)
  xbee_com = SerialFlow(Serial2, flow_pin)
  xbee_ser = Serial2

  # Winch and Rudder setup
  global winch, rudder
  winch = Winch(winch_pwm)
  rudder = Rudder(rudder_pwm)

  # GPS interaction setup
  global gps_reader
  # TODO: Which serial port?
  Serial4.begin(9600)
  gps_reader = GPS(Serial4)

  # Arduino itneraction setup
  global arduino
  # TODO: Which serial port?
  Serial1.begin(9600)
  arduino = ArduinoInterface(Serial1)

  # Waypoints setup
  waypoints = []
  try:
    with open("waypoints") as f:
      for line in f:
        stringNums = line.split(",")
        waypoints.append((float(stringNums[0]),float(stringNums[1])))
  except:
    pass
  print waypoints
  delay(5000)

heading = 0
goal_lat = 0
goal_lon = 0
m_per_deg_lat = 111000.
m_per_deg_lon = 82500.

lat_fieldm = 42.274015
lon_fieldm = -71.811769

def loop():
  global xbee_ser
  global gps_reader
  global goal_lat, goal_lon
  global heading
  lat, lon = gps_reader.get_pos()
  print "Latitude, Longitude:"
  print lat, ", ", lon
  goal_lat = lat_fieldm
  goal_lon = lon_fieldm
  heading = haversine.heading((lat,lon), (goal_lat, goal_lon))
  print "heading ", heading
  xbee_ser.prints("%f, %f\n" % (lat, lon))

  global arduino
  rel_wind = arduino.get_wind()
  roll, yaw = arduino.get_orientation()
  print "PPM, Wind, Roll/Yaw, Boom/Rudder"
  print arduino.get_ppm()
  print rel_wind
  print roll, yaw
  print arduino.get_pots()

  global winch, rudder
  winch_val = get_winch_angle(rel_wind)
  rudder_val = get_rudder_angle(heading, yaw, rel_wind * pi / 180.)
  print "Winch, Rudder"
  print winch_val, rudder_val
  winch.set(winch_val)
  rudder.set(rudder_val)

  xbee_ser.prints("HELLO, WORLD!!!!" + repr(lon));
  delay(20)

run(setup, loop)
