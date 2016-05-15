#!/usr/bin/python
from bbio import *
from gps import GPS
from ard_int import ArduinoInterface

def setup():

  # GPS interaction setup
  global gps_reader
  # TODO: Which serial port?
  Serial4.begin(9600)
  gps_reader = GPS(Serial4)

def loop():
  global gps_reader
  lat, lon = gps_reader.get_pos()
  print "Latitude, Longitude:"
  print lat, ", ", lon
  delay(20)

run(setup, loop)
