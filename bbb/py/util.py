from math import pi

def angle_sum(t1, t2):
  total = t1 + t2
  while total > pi: total -= 2 * pi
  while total < -pi: total += 2 * pi
  return total

def angle_diff(t1, t2):
  return angle_sum(t1, -t2)

def angle_diff_deg(t1, t2):
  diff = t1 - t2
  while diff > 180: diff -= 360
  while diff < -180: diff += 360
  return diff
