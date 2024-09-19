#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <string>
#include <sstream>

#include "v5.h"
#include "v5_vcs.h"

#include "recording.h"

using namespace vex;

#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++


// A global instance of competition
competition Competition = competition();

// The controller
controller Controller = controller();
// The controller's deadzone
const float deadzone = 10.1f;

// The brain
brain Brain = brain();

// The motor's base speed.
const float speed = 0.5f;

// define your global instances of motors and other devices here
motor leftBackMotor = motor(PORT6, true);
motor leftFrontMotor = motor(PORT18, true);
motor rightBackMotor = motor(PORT7, true);
motor rightFrontMotor = motor(PORT5, true);

motor_group leftMotors = motor_group(leftBackMotor, leftFrontMotor);
motor_group rightMotors = motor_group(rightBackMotor, rightFrontMotor);