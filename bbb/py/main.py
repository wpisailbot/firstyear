from bbio import *
from math import pi
from serial_flow import SerialFlow
from motors import Rudder, Winch
from gps import GPS
from ard_int import ArduinoInterface
from controls import get_rudder_angle, get_winch_angle

flow_pin = None
winch_pwm = PWM1A
rudder_pwm = PWM1B
def setup():

  # XBee setup
  global xbee_com
  Serial2.begin(9600)
  xbee_com = SerialFlow(Serial2, flow_pin)

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

def loop():
  global gps_reader
  lat, lon = gps_reader.get_pos()
  print "Latitude, Longitude:"
  print lat, ", ", lon

  global arduino
  rel_wind = arduino.get_wind()
  roll, yaw = arduino.get_orientation()
  print "PPM, Wind, Roll/Yaw, Boom/Rudder"
  print arduino.get_ppm()
  print rel_wind
  print roll, yaw
  print arduino.get_pots()

  global winch, rudder
  heading = 0
  winch.set(get_winch_angle(rel_wind))
  rudder.set(get_rudder_angle(heading, yaw, rel_wind * pi / 180.));

run(setup, loop)
