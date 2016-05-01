#!/usr/bin/python
import pylab
import numpy
# Line begins with: Value
# "W pot: ": pot value from boom (0-1023).
# "Winch: ": microseconds written to winch servo.
# "roll yaw ": radians of roll and yaw. For roll, 0=upright.
# "Rudder": Not useful (since neither rudder pot nor rudder servo are plugged in)
# "$GPRMC": GPS Data.
# Just a raw number with no context: Wind sensor reading (degrees).
fname = "auto_winch1.log"
def show_gps():
  x = []
  y = []
  with open(fname) as f:
    for line in f:
      start = line.find("GPRMC")
      info = line[start:].split(",")
      if len(info) < 5:
        continue
      if info[5] and info[3]:
        x.append(float(info[5][:3]) + float(info[5][3:]) / 60.)
        y.append(float(info[3][:2]) + float(info[3][2:]) / 60.)
        print "%f, -%f" % (y[-1], x[-1])

  pylab.plot(x, y)
  pylab.show()

def winch_data():
  wmicro = []
  wpot = []
  # Use to prevent one of the arrays getting ahead of the other.
  got_micro_last = False
  with open(fname) as f:
    for line in f:
      try:
        microstart = line.find("Winch: ")
        potstart = line.find("W pot: ")
        if microstart != -1 and not got_micro_last:
          wmicro.append(int(line[7:]))
          got_micro_last = True
        elif potstart != -1 and got_micro_last:
          wpot.append(int(line[7:]))
          got_micro_last = False
      except:
        pass
  pylab.ylim([0, 2000])
  pylab.plot(range(len(wmicro)), wmicro, label="Microseconds")
  pylab.plot(range(len(wpot)), wpot, label="Potentiometer")
  pylab.legend()
  pylab.show()

def wind_data():
  # I'm not sure about the yaw data. The IMU may not have calibrated completely
  wind = []
  roll = []
  yaw = []
  with open(fname) as f:
    for line in f:
      try:
        # If line is just a number, it is the wind.
        windval = int(line)
        wind.append(windval * numpy.pi / 180.)
      except: pass
      try:
        if line.find("roll ") != -1:
          arr = line.split(' ');
          roll.append(float(arr[1]))
          yaw.append(float(arr[3]))
      except: pass
  pylab.ylim([-numpy.pi, 2*numpy.pi])
  pylab.plot(range(len(wind)), wind, label="Wind")
  pylab.plot(range(len(roll)), roll, label="Roll")
  pylab.plot(range(len(yaw)), yaw, label="Yaw")
  pylab.legend()
  pylab.show()

show_gps()
