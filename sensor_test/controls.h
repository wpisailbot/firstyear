#pragma once
#include "util.h"
#include <stdio.h>

/**
 * Get the rudder angle given current state and goal information.
 * goal_heading and yaw should be the same frame--ie, for both, positive
 * should be CCW, negative CW, and if yaw == goal_heading, we should be
 * heading in our goal direction.
 * @param goal_heading The goal heading.
 * @param yaw The current heading of the boat.
 * @param rel_wind The current apparent wind, 0=if we are going upwind.
 */
int get_rudder_angle(double goal_heading, double yaw, double rel_wind) {
  double heading_diff = angle_diff(goal_heading, yaw);
  // Logic so that our goal heading doesn't send us into the wind.
  // Remember, reading = 0 is into the wind.
  double wind_reading = rel_wind;
  // The position of the wind relative to the current heading.
  // Used to determine which side of the wind we are attempting to go to.
  double heading_wind = angle_diff(heading_diff, wind_reading);
  // How close we are allowed to sail to the wind.
  double max_close_haul = PI / 6;
  double max_head_diff = angle_sum(wind_reading, max_close_haul);
  double min_head_diff = angle_diff(wind_reading, max_close_haul);
  printf("heading_diff: %f\n", heading_diff);

  if (heading_diff > min_head_diff && heading_wind <= 0)
    heading_diff = min_head_diff;
  else if (heading_diff < max_head_diff && heading_wind > 0)
    heading_diff = max_head_diff;
  printf("heading_diff: %f\n", heading_diff);
  // Now, calculate rudder angle
  double kPrudder = 20.0;
  int write_val = kPrudder * heading_diff + 90;
  return write_val;
}
