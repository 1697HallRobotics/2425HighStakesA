#include "api.h"

using namespace pros;

#define MACRO_WALLGOAL  1ull
#define MACRO_UNDEF1    2ull
#define MACRO_UNDEF2    4ull
#define MACRO_UNDEF3    8ull
#define MACRO_UNDEF4    16ull
#define MACRO_UNDEF5    32ull
#define MACRO_UNDEF6    64ull
#define MACRO_UNDEF7    128ull
#define MACRO_UNDEF8    256ull
#define MACRO_UNDEF9    512ull
#define MACRO_UNDEF10   1024ull
#define MACRO_UNDEF11   2048ull
#define MACRO_UNDEF12   4096ull

uint64_t RUNNING_MACROS = 0;

#define FLAG_MACRO_ON(macro) RUNNING_MACROS |= macro
#define FLAG_MACRO_OFF(macro) RUNNING_MACROS &= ~macro
#define MACRO_RUNNING(macro) RUNNING_MACROS & macro

#define REF_MACRO(name) macro_##name
#define MACRO_IMPLEMENTATION(name) REF_MACRO(name)(void *param)
#define DEFINE_MACRO(name) void MACRO_IMPLEMENTATION(name)
#define TRIGGER_MACRO(name) pros::rtos::Task macroTask__##name(REF_MACRO(name))

// The controller
Controller controller(E_CONTROLLER_MASTER);

// The controller's deadzone
const float deadzone = 10.1f;

// The motor's base speed, as a multiplier.
const float speed = 1.0f;

const int8_t leftBackPort   = 11;
const int8_t leftFrontPort  = 5;
const int8_t rightBackPort  = 6;
const int8_t rightFrontPort = 3;
const int8_t liftPort       = 14;
const int8_t clampPort      = 16;
const int8_t intakePort     = 0; // not implemented yet


Motor leftBackMotor   (leftBackPort);
Motor leftFrontMotor  (leftFrontPort);
Motor rightBackMotor  (rightBackPort);
Motor rightFrontMotor (rightFrontPort);
Motor liftMotor       (liftPort);
Motor clampMotor      (clampPort);
Motor intakeMotor     (intakePort);

MotorGroup leftMotors ({leftBackPort, leftFrontPort});
MotorGroup rightMotors({rightBackPort, rightFrontPort});

adi::Pneumatics clampPneumatics = adi::Pneumatics('a', false);