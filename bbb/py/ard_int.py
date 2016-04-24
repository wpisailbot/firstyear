from bbio import *
import thread

class ArduinoInterface(object):
  def __init__(self, ser):
    """
      params:
        ser: A serial object. ser.begin() should have already been called.
    """
    self.ser = ser
    self.ppm = []
    self.wind = 0
    self.roll = 0
    self.yaw = 0
    self.winch = 0
    self.rudder = 0
    self.lock = thread.allocate_lock()
    self.run_thread = thread.start_new_thread(self.run, ())

  def run(self):
    """
      Spawn a new thread which continually updates things.
    """
    while True:
      buf = ""
      while len(buf) == 0 or buf[-1] != '\n':
        if self.ser.available(): buf += self.ser.read()
        else: delay(1) # Avoid pegging CPU

      tokens = buf.split(' ')
      s = tokens[0]
      self.lock.acquire()
      try:
        if s == "PPM":
          self.ppm = [int(i) for i in tokens[1:]]
        elif s == "Wind":
          self.wind = int(tokens[1])
        elif s == "roll":
          self.roll = float(tokens[1])
        elif s == "yaw":
          self.yaw = float(tokens[1])
        elif s == "Wpot":
          self.winch = int(tokens[1])
        elif s == "Rpot":
          self.rudder = int(tokens[1])
      except: pass # A cast likely failed
      self.lock.release()

  def get_ppm(self):
    ppm = []
    self.lock.acquire()
    ppm = list(self.ppm)
    self.lock.release()
    return ppm

  def get_wind(self):
    wind = 0
    self.lock.acquire()
    wind = self.wind
    self.lock.release()
    return wind

  def get_orientation(self):
    """
      returns: roll, yaw
    """
    roll = 0
    yaw = 0
    self.lock.acquire()
    roll = self.roll
    yaw = self.yaw
    self.lock.release()
    return roll, yaw

  def get_pots(self):
    """
      returns: boom, rudder
    """
    winch = 0
    rudder = 0
    self.lock.acquire()
    winch = self.winch
    rudder = self.rudder
    self.lock.release()
    return winch, rudder
