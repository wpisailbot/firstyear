from bbio import *
from bbio.libraries.Servo import *

class Winch(object):
  def __init__(self, winch_port):
    self.winch = Servo(winch_port, min_ms=1.2, max_ms=1.7)

  def set(self, val):
    self.winch.write(val)

class Rudder(object):
  def __init__(self, rudder_port):
    self.rudder = Servo(rudder_port)

  def set(self, val):
    self.rudder.write(val)
