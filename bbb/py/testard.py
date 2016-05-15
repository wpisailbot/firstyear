#!/usr/bin/python
from bbio import *

ser = Serial1
ser.begin(9600)


while True:
  buf = ''
  while len(buf) == 0 or buf[-1] != '\r':
    if ser.available(): buf += ser.read()
    else: delay(1) # Avoid pegging CPU
  print buf
