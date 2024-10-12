#include "main.h"
#include "vex.h"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	lcd::initialize();
	lcd::set_text(3, "                _    _ ");
	lcd::set_text(4, "                o    o ");
	lcd::set_text(5, "               |______|");

	rightMotors.set_brake_mode_all(MotorBrake::brake);
	leftMotors.set_brake_mode_all(MotorBrake::brake);

	mysteryMotor.set_brake_mode(MotorBrake::hold);

	mysteryMotor.set_zero_position(0);
	rightMotors.set_zero_position_all(0);
	leftMotors.set_zero_position_all(0);
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
	virtual_controller* vcontroller = begin_playback("lucastest2");
	if (vcontroller == nullptr)
		return;

	while (1)
	{
		// clamp the controller from 0-100
		float turnPower = vcontroller->get_analog(ANALOG_LEFT_X);
		float forwardPower = vcontroller->get_analog(ANALOG_LEFT_Y);

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

		if (vcontroller->get_digital_new_press(DIGITAL_R1))
			mysteryMotor.move_absolute(400, 75);
		else if (vcontroller->get_digital_new_press(DIGITAL_R2))
			mysteryMotor.move_absolute(5, 50);

		task_delay(1);
		// i say let the kernel starve i need precision
	}
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
	start_recording("lucastest2", 20);

	while (1)
	{
		// clamp the controller from 0-100
		float turnPower = controller.get_analog(ANALOG_LEFT_X);
		float forwardPower = controller.get_analog(ANALOG_LEFT_Y);

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

		if (controller.get_digital_new_press(DIGITAL_R1))
			mysteryMotor.move_absolute(400, 75);
		else if (controller.get_digital_new_press(DIGITAL_R2))
			mysteryMotor.move_absolute(5, 50);

		task_delay(1);
		// i say let the kernel starve i need precision
	}
}