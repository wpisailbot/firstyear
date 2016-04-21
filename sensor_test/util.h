#pragma once
#include <math.h>
#ifndef PI
#define PI 3.1415926536
#endif

double angle_sum(double t1, double t2) {
  double sum = t1 + t2;
  while (sum > PI) sum -= 2 * PI;
  while (sum < -PI) sum += 2 * PI;
  return sum;
}

double angle_diff(double t1, double t2) {
  double diff = t1 - t2;
  while (diff > PI) diff -= 2 * PI;
  while (diff < -PI) diff += 2 * PI;
  return diff;
}

int angle_diff_deg(int t1, int t2) {
  int diff = t1 - t2;
  while (diff > 180) diff -= 360;
  while (diff < -180) diff += 360;
  return diff;
}
