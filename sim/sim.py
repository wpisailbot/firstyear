#!/usr/bin/python

import math
import pylab

class State(object):
  """
    The state of a boat (current pose, velocities, environment conditions, etc.)
    For now, assume:
      -a simple cat-rig.
      -No currents in water.
      -See comments.
  """
  def __init__(self):

    # Pose/velocity variables.
    # All in meters, radians, and seconds.
    self.x = 0
    self.y = 0
    self.vx = 0
    self.vy = 0
    # Note on angles/transformations:
    # Angle apply in a generally intuitive manner. This is simplified somewhat
    # as we are ignoring the pitch angle of the boat. theta (ie, the yaw) refers
    # to the direction that an imaginary line straight along the length of the
    # hull would go. heel (the roll) refers to the orientation of the boat
    # around the same imaginary line. As such, at +/- PI heel, the boat is
    # lying on its side in the water.
    self.theta = 0 # NOT compass heading. Compasses are stupid. Go counter-clockwise.
    self.omega = 0 # Rate of change of yaw/angle.
    # For now, ignore dynamics of heeling (ie, assume it is static).
    self.heel = 0 # Equivalent to roll; 0 = straight up, +PI = sails in water to port.

    # Internal State
    self.main = 0 # 0 = in line with boat, +PI = sticking out to starboard.
    self.rudder = 0 # 0 = in line with boat, +PI = sticking out to starboard.

    # Environmental Variables.
    self.wv = 0 # Wind speed
    self.wdir = 0 # Wind heading; 0 = From north; +PI = From West.

    # General boat info:
    self.m = 3 # kg
    self.cg = -0.5 # Distance of cg below waterline.
    # For sake of simplicity, we will assume that the bouyancy always operates
    # in such a way as to introduce no additional torques on the boats while
    # perfectly counteracting gravity in the z-direction.

    # Fluids related constants.

    # C constant as described in Boat.calc_heel_angle
    # Number used ecause it produces semi-sensible results.
    self.heel_c = .5

    # Drag constant for hull traveling through the water.
    # Number essentially made up.
    self.hull_c = 5 # kg / m

    # conversion between rudder andgle and boat yaw rate.
    self.rudder_c = 0.2

def clip_angle(angle):
  """
    Returns an angle clipped to +/-PI.
  """
  while angle > math.pi:
    angle -= math.pi * 2
  while angle < -math.pi:
    angle += math.pi * 2
  return angle

class Boat(object):
  """
    Boat object for simulation.
  """

  def __init__(self):
    """
      Initializes boat object; nothing too fancy.
    """
    self.state = State()

  def calc_sail_alpha(self):
    """
      Calculates the sail's angle of attack to the wind, in radians.
    """
    alpha = self.state.wdir - self.state.main - self.state.theta
    return clip_angle(alpha)

  def calc_heel_angle(self):
    """
      Calculates the equilibrium point for where the heeling torques due
      to gravity and wind balance each other out.

      The math:
        Torques must be equal.
        For gravity:
          l is the distance of the cg above the axis of rotation.
            Keep in mind that with ballast below the waterline, l < 0
          t_g = m * g * sin(heel) * l
        For the wind, we will assume an extremely simplistic form of drag:
          C is a constant and v_w is wind speed perpendicular to the sail.
          The C constant incorporates the height of the center of effort for the sail.
          t_w = C * v_w ^ 2 * cos(heel)
        If we set these equations equal and isolate heel, we get:
          tan(heel) = C * v_w ^ 2 / (m * g * l)
          heel = atan2(C * v_w ^ 2, m * g * l)
    """
    tany = self.state.heel_c * self.state.wv ** 2
    tanx = self.state.m * 9.8 * self.state.cg
    return math.atan2(tany, tanx)

  def calc_water_polar(self):
    """
      Calculates relative water velocity into polar values.
      Returns:
        speed: Absolute speed of water in m/s
        direction: Direction that the boat is headed in the
          water, in radians.
    """
    x = self.state.vx
    y = self.state.vy
    #print "vx, vy, theta: ",  x, y, self.state.theta
    speed = math.sqrt(x ** 2 + y ** 2)
    angle = math.atan2(y, x)
    return speed, angle

  def calc_hull_drag(self):
    """
      Calculate rough hull drag (ignores wave resistance) with many assumptions.
    """
    waterv, watert = self.calc_water_polar()
    magnitude = -self.state.hull_c * waterv ** 2
    #print "Water angle: ", watert
    sint = math.sin(watert)
    cost = math.cos(watert)
    #sint = math.sin(self.state.theta)
    #cost = math.cos(self.state.theta)
    return cost * magnitude, sint * magnitude

  def calc_rudder_speed(self):
    """
      Calculates the yaw speed of the boat based on the rudder angle.
      Assumes a linear relationship between rudder angle and yaw rate,
        with no other factors involved.
    """
    return self.state.rudder_c * self.state.rudder

  def calc_sail_forces(self, alpha=None):
    """
      We are going to ignore the keel forces, as the drag forces can be
      incorporated into the hull drag and the lift can be assumed to
      counteract the sail perfectly (perhaps we will add in some leeward
      movement later).
      If alpha is provided, internal state is ignored.
      Return (drive, lat) forces from the sails.
    """
    # Assume lift coefficient is composed of two lines, meeting at the top
    # at the 30 degree angle of attack, with the first starting at 10 deg
    # and the second to 110 degrees.
    if alpha == None: alpha = self.calc_sail_alpha()
    #print "Sail angle of attack: ", alpha
    sign = -1 if alpha < 0 else 1
    alpha = abs(alpha)
    start = 10. * math.pi / 180.
    middle = 30. * math.pi / 180.
    end = 110. * math.pi / 180.
    highest_c = 4
    lift_c_1 = (alpha - start) * highest_c / (middle - start)
    lift_c_2 = (end - alpha) * highest_c / (end - middle)
    lift_c = lift_c_1 if alpha < middle else lift_c_2

    # Use a quadratic fit for drag equation, because why not.
    drag_c = 3. * (3. / 16000. * alpha ** 2 - 6. / 1000. * alpha + .21625)

    drag = drag_c * self.state.wv ** 2
    lift = sign * lift_c * self.state.wv ** 2

    sinmain = math.sin(self.state.main)
    cosmain = math.cos(self.state.main)
    f_drive = lift * sinmain + drag * cosmain
    f_lat = lift * cosmain + drag * sinmain

    return f_drive, f_lat

  def calc_accel(self):
    """
      Calculates and returns the acceleration of the boat. Returns (x, y).
    """
    sail_drive, _ = self.calc_sail_forces()
    hull_drag = self.calc_hull_drag()
    #print "sails, hull: ", sail_drive, hull_drag
    #accel = (sail_drive + hull_drag) / self.state.m
    sina = math.sin(self.state.theta)
    cosa = math.cos(self.state.theta)
    sailx = cosa * sail_drive
    saily = sina * sail_drive
    accelx = (hull_drag[0] + sailx) / self.state.m
    accely = (hull_drag[1] + saily) / self.state.m
    return accelx, accely
    #return sina * accel, cosa * accel

  def set_control(self, main, rudder):
    """
      Sets the mainsail and rudder angles for the boat.
    """
    self.state.main = main
    self.state.rudder = rudder

  def set_wind(self, wv, wdir):
    """
      Sets the wind information.
    """
    self.state.wv = wv
    self.state.wdir = wdir

  def update(self, dt=0.01):
    """
      Updates all the internal state using the various calculation functions.
    """
    ax, ay = self.calc_accel()
    self.state.x += self.state.vx * dt + 0.5 * ax * dt ** 2
    self.state.y += self.state.vy * dt + 0.5 * ay * dt ** 2
    self.state.vx += ax * dt
    self.state.vy += ay * dt

    self.state.omega = self.calc_rudder_speed()
    self.state.theta += self.state.omega * dt
    self.state.theta = clip_angle(self.state.theta)

    self.state.heel = self.calc_heel_angle()

def beat(boat):
  boat.set_wind(3, math.pi * 2 / 4)
  boat.state.theta = -math.pi / 4
  main_ctl = math.pi / 3
  rudder_set = math.pi / 4
  boat.set_control(main_ctl, 0)

  pointx = []
  pointy = []
  tacking = False
  prev_theta_small = True
  try:
    for _ in xrange(45000):
      boat.update()
      pointx.append(boat.state.x)
      pointy.append(boat.state.y)
      x_cutoff = 15
      if tacking and (boat.state.theta < -math.pi * 3 / 4 or boat.state.theta > -math.pi / 4):
        boat.set_control(boat.state.main, 0)
        tacking = False
      elif tacking and ((boat.state.theta > -math.pi / 2 and not prev_theta_small) or
                        (boat.state.theta < -math.pi / 2 and prev_theta_small)):
        boat.set_control(-boat.state.main, boat.state.rudder)
        prev_theta_small = not prev_theta_small
      elif boat.state.x > x_cutoff and prev_theta_small:
        boat.set_control(boat.state.main, -rudder_set)
        tacking = True
      elif boat.state.x < -x_cutoff and not prev_theta_small:
        boat.set_control(boat.state.main, rudder_set)
        tacking = True
  except Exception as exc:
    print exc

  pylab.plot(pointx, pointy)

  pylab.show()

def tack(boat):
  boat.set_wind(3, math.pi * 2 / 4)
  boat.state.theta = math.pi / 4
  main_ctl = 0#math.pi / 3
  rudder_set = math.pi / 4
  boat.set_control(main_ctl, 0)

  pointx = []
  pointy = []
  pointt = []
  tacking = False
  prev_theta_small = True
  try:
    for _ in xrange(45000):
      boat.update()
      pointx.append(boat.state.x)
      pointy.append(boat.state.y)
      pointt.append(boat.state.theta * 20)
      x_cutoff = 15
      if tacking and (boat.state.theta > math.pi * 3 / 4 or boat.state.theta < math.pi / 4):
        boat.set_control(boat.state.main, 0)
        tacking = False
      elif tacking and ((boat.state.theta < math.pi / 2 and not prev_theta_small) or
                        (boat.state.theta > math.pi / 2 and prev_theta_small)):
        boat.set_control(-boat.state.main, boat.state.rudder)
        prev_theta_small = not prev_theta_small
      elif boat.state.x > x_cutoff and prev_theta_small:
        boat.set_control(boat.state.main, rudder_set)
        tacking = True
      elif boat.state.x < -x_cutoff and not prev_theta_small:
        boat.set_control(boat.state.main, -rudder_set)
        tacking = True
  except Exception as exc:
    print exc

  pylab.plot(pointx, pointy)
  #pylab.plot(pointx, pointt)

  pylab.show()

if __name__ == "__main__":
  boat = Boat()
  beat(boat)
