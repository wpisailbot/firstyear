from util import *
from math import pi

def get_rudder_angle(goal_heading, yaw, rel_wind):
  """
   Get the rudder angle given current state and goal information.
   goal_heading and yaw should be the same frame--ie, for both, positive
   should be CCW, negative CW, and if yaw == goal_heading, we should be
   heading in our goal direction.
   params:
     goal_heading: The goal heading.
     yaw: The current heading of the boat.
     rel_wind: The current apparent wind, 0=if we are going upwind.
  """
  heading_diff = angle_diff(goal_heading, yaw)
  heading_wind = angle_diff(heading_diff, rel_wind)
  max_close_haul = pi / 4
  max_head_diff = angle_sum(rel_wind, max_close_haul)
  min_head_diff = angle_diff(rel_wind, max_close_haul)

  if heading_diff > min_head_diff and heading_wind <= 0:
    heading_diff = min_head_diff
  elif heading_diff < max_head_diff and heading_wind > 0:
    heading_diff = max_head_diff

  kPrudder = 20.
  return kPrudder * -heading_diff + 90

def get_winch_angle(rel_wind):
  """
    params:
      rel_wind: The relative wind; 0=upwind, +/-180=downwind.
    returns:
      winch value to write (0-180). 0 = all the way out, 180=all in.
  """
  return 180-abs(rel_wind)
