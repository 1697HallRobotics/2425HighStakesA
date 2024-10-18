#include "main.h"
#include "pros/misc.h"
#include <sys/time.h>
#include <time.h>
#include "math.h"

int16_t xVelo = 0;
int16_t yVelo = 0;

template <typename T> int sign(T val) {
	int ret = (T(0) < val) - (val < T(0));
    return ret == 0 ? 1 : ret;
}

void update_dvd(lv_timer_t* timer)
{
	if (xVelo == 0) xVelo = (rand() % 4);
	if (yVelo == 0) yVelo = (rand() % 4);

	int16_t nextX = lv_obj_get_x(dvd_img) + xVelo;
	int16_t nextY = lv_obj_get_y(dvd_img) + yVelo;

	if (nextX < 0 || nextX > 293) {
		xVelo = -(rand() % 4) * sign(xVelo);
	}

	if (nextY < 0 || nextY > 157) {
		xVelo += rand() % 2 * sign(xVelo);
		yVelo = -(rand() % 4) * sign(yVelo);
	} 

	lv_obj_set_x(dvd_img, lv_obj_get_x(dvd_img) + xVelo);
	lv_obj_set_y(dvd_img, lv_obj_get_y(dvd_img) + yVelo);
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	xVelo = rand() % 4;
	yVelo = rand() % 4;
	dvd_img = lv_img_create(lv_scr_act());
	lv_img_set_src(dvd_img, &dvd_logo);

	lv_timer_create(update_dvd, 33, NULL);

	rightMotors.set_brake_mode_all(MotorBrake::brake);
	leftMotors.set_brake_mode_all(MotorBrake::brake);

	liftMotor.set_brake_mode(MotorBrake::hold);
	clampMotor.set_brake_mode(MotorBrake::hold);

	intakeMotor.set_brake_mode(MotorBrake::hold);

	liftMotor.set_zero_position(0);
	clampMotor.set_zero_position(0);
	rightMotors.set_zero_position_all(0);
	leftMotors.set_zero_position_all(0);
	intakeMotor.set_zero_position(0);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

void autonomous_LAMEANDSTINKY()
{
	rightMotors.move(-127);
	leftMotors.move(-127);

	task_delay(500);

	rightMotors.brake();
	leftMotors.brake();

	clampPneumatics.toggle();

	task_delay(500);

	rightMotors.move(127);
	leftMotors.move(127);

	task_delay(500);

	rightMotors.brake();
	leftMotors.brake();
}

void autonomous_cool()
{
	virtual_controller* vcontroller = begin_playback("lucastest2");
	if (vcontroller == nullptr)
		return;

	#define CONTROLLER (*vcontroller)

	while (1)
	{
		// clamp the controller from 0-100
		float turnPower = CONTROLLER.get_analog(ANALOG_LEFT_X);
		float forwardPower = CONTROLLER.get_analog(ANALOG_LEFT_Y);

		if (fabsf(turnPower) <= deadzone)
			turnPower = 0;
		if (fabsf(forwardPower) <= deadzone)
			forwardPower = 0;

		if (turnPower != 0 || forwardPower != 0)
		{
			rightMotors.move((turnPower - forwardPower) * speed);
			leftMotors.move((turnPower + forwardPower) * speed);
		}
		else
		{
			rightMotors.brake();
			leftMotors.brake();
		}

		if (CONTROLLER.get_digital_new_press(DIGITAL_R1))
			liftMotor.move_absolute(400, 75);
		else if (CONTROLLER.get_digital_new_press(DIGITAL_R2))
			liftMotor.move_absolute(5, 50);

		if (CONTROLLER.get_digital(DIGITAL_L1))
			clampMotor.move(40);
		else if (CONTROLLER.get_digital(DIGITAL_L2))
			clampMotor.move(-40);
		else
			clampMotor.brake();

		if (CONTROLLER.get_digital_new_press(DIGITAL_A)) TRIGGER_MACRO(ScoreWallGoal);

		if (CONTROLLER.get_digital_new_press(DIGITAL_B))
			clampPneumatics.toggle();

		task_delay(1);
	}

	#undef CONTROLLER
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() 
{
	autonomous_cool();
	//autonomous_LAMEANDSTINKY();
}

void MACRO_IMPLEMENTATION(ScoreWallGoal)
{
	if (MACRO_RUNNING(MACRO_WALLGOAL)) return;
	FLAG_MACRO_ON(MACRO_WALLGOAL);

	liftMotor.move_absolute(5, 50);

	// code here
	intakeMotor.move(127);

	task_delay(4000);

	intakeMotor.brake();

	liftMotor.move_absolute(400, 75);

	task_delay(500);

	intakeMotor.move(-127);

	task_delay(4000);

	intakeMotor.brake();

	task_delay(500);

	liftMotor.move_absolute(5, 50);

	FLAG_MACRO_OFF(MACRO_WALLGOAL);
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol()
{
	//start_recording("lucastest2", 20);

	// this is used for autonomous to easily swap out the controller with the virtual controller
	#define CONTROLLER controller

	while (1)
	{
		// clamp the controller from 0-100
		float 	turnPower = CONTROLLER.get_analog(ANALOG_LEFT_X),
		      	forwardPower = CONTROLLER.get_analog(ANALOG_LEFT_Y);

		if (fabsf(turnPower) <= deadzone)
			turnPower = 0;
		if (fabsf(forwardPower) <= deadzone)
			forwardPower = 0;

		if (turnPower != 0 || forwardPower != 0)
		{
			rightMotors.move((turnPower - forwardPower) * speed);
			leftMotors.move((turnPower + forwardPower) * speed);
		}
		else
		{
			rightMotors.brake();
			leftMotors.brake();
		}

		if (CONTROLLER.get_digital_new_press(DIGITAL_R1))
			liftMotor.move_absolute(400, 75);
		else if (CONTROLLER.get_digital_new_press(DIGITAL_R2))
			liftMotor.move_absolute(5, 50);

		if (CONTROLLER.get_digital(DIGITAL_L1))
			clampMotor.move(40);
		else if (CONTROLLER.get_digital(DIGITAL_L2))
			clampMotor.move(-40);
		else
			clampMotor.brake();

		if (CONTROLLER.get_digital_new_press(DIGITAL_A))
			TRIGGER_MACRO(ScoreWallGoal);

		if (CONTROLLER.get_digital_new_press(DIGITAL_B))
			clampPneumatics.toggle();

		task_delay(1);

		lv_timer_handler();
	}

	#undef CONTROLLER
}