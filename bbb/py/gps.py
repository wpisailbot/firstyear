from bbio import *
import thread

class GPS(object):
  def __init__(self, ser):
    """
      params:
        ser: A serial object. ser.begin() should have already been called.
    """
    self.ser = ser
    self.lat = 0
    self.lon = 0
    self.lock = thread.allocate_lock()
    self.run_thread = thread.start_new_thread(self.run, ())

  def run(self):
    """
      Spawn a new thread which continually updates things.
    """
    while True:
      buf = ""
      while len(buf) == 0 or buf[-1] != '\r':
        if self.ser.available(): buf += self.ser.read()
        else: delay(1) # Avoid pegging CPU

      start = buf.find("$GPGGA")
      # Split starting at the $GPGGA, ignore the GPGGA itself.
      tokens = buf[start:].split(',')[1:]
      self.lock.acquire()
      try:
        # lat_str = ddmm.mmmmm
        lat_str = tokens[1]
        self.lat = int(lat_str[0:2]) + float(lat_str[2:]) / 60.
        # lon_str = dddmm.mmmmm
        lon_str = tokens[3]
        self.lon = int(lon_str[0:3]) + float(lon_str[3:]) / 60.
      except: pass
      self.lock.release()

  def get_pos(self):
    """ returns: latitude, longitude """
    lat = 0
    lon = 0
    self.lock.acquire()
    lat = self.lat
    lon = self.lon
    self.lock.release()
    return lat, lon
