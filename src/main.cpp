/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       HallRobotics1697                                          */
/*    Created:      9/6/2024, 1:52:46 PM                                      */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

template < typename T > std::string to_string( const T& n )
{
    std::ostringstream stm ;
    stm << n ;
    return stm.str();
}

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void)
{

	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
	Brain.Screen.print("Hardware threads available: %d (min required: 2)", thread::hardware_concurrency());
	
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void)
{
	// ..........................................................................
	// Insert autonomous user code here.
	// ..........................................................................
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void)
{
	/*
	start_recording(string("lucastest1"), &Brain, &Controller, 20);
	
	// User control code here, inside the loop
	while (1)
	{
		float leftPower = Controller.Axis1.position();
		float rightPower = Controller.Axis3.position();

		if (fabsf(leftPower) <= deadzone)
			leftPower = 0;
		if (fabsf(rightPower) <= deadzone)
			rightPower = 0;

		if (leftPower != 0 || rightPower != 0)
		{
			rightMotors.spin(fwd, (leftPower - rightPower) * speed, pct);
			leftMotors.spin(fwd, (leftPower + rightPower) * speed, pct);
		}
		else
		{
			leftMotors.stop(brake);
			rightMotors.stop(brake);
		}
	}
	*/

	
	
	virtual_controller* vcontroller = begin_playback(string("lucastest1"), &Brain);

	if (vcontroller == nullptr) return;
	
	while (true) {
		float leftPower = (*vcontroller).Axis1.position();
		float rightPower = (*vcontroller).Axis3.position();

		if (fabsf(leftPower) <= deadzone)
			leftPower = 0;
		if (fabsf(rightPower) <= deadzone)
			rightPower = 0;

		if (leftPower != 0 || rightPower != 0)
		{
			rightMotors.spin(fwd, (leftPower - rightPower) * speed, pct);
			leftMotors.spin(fwd, (leftPower + rightPower) * speed, pct);
		}
		else
		{
			leftMotors.stop(brake);
			rightMotors.stop(brake);
		}
	}
	
	
}

//
// Main will set up the competition functions and callbacks. :)
//
int main()
{
	// Set up callbacks for autonomous and driver control periods.
	Competition.autonomous(autonomous);
	Competition.drivercontrol(usercontrol);

	// Run the pre-autonomous function.
	pre_auton();

	// Prevent main from exiting with an infinite loop.
	while (true)
	{
		wait(5, msec);
	}
}
