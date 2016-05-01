#!/usr/bin/python
import pylab

fname = "auto_log6.log"
def show_gps():
  x = []
  y = []
  with open(fname) as f:
    for line in f:
      info = line.split(",")
      if len(info) != 2: continue
      try:
        xi = float(info[0])
        yi = float(info[1])
        if xi == 0 and yi == 0: continue
        if len(x) != 0 and xi == x[-1] and yi == y[-1]: continue
        yi = -yi
        print "%f, %f" % (xi, yi)
        x.append(xi)
        y.append(yi)
      except: pass

  pylab.plot(y, x, marker='o')
  pylab.show()

show_gps()
