from bbio import *

class SerialFlow(object):
  def __init__(self, ser, flow_pin):
    """
      params:
        ser: One of the serial objects (eg, Serial2). begin() has already been called.
        flow_pin: Pin to use for flow control. Unfortunately,
          I have been unable to get Hardware flow control working on the
          BBB, so I am just manually checking a pin.
    """
    self.ser = ser
    self.flow = flow_pin
    if flow_pin != None:
      pinMode(flow_pin, INPUT)

  def prints(self, data):
    if flow_pin != None:
      while digitalRead(self.flow):
        continue
    self.ser.prints(data)
