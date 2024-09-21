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
	//Brain.Screen.clearScreen();

	start_recording(string("test5"), &Brain, &Controller, 20);
	/*
	virtual_controller* vcontroller = begin_playback(string("test4"), &Brain);
	
	while (true) {
		Brain.Screen.printAt(20, 20,   "axis1:%d    ",(signed int)(*vcontroller).Axis1.position());
		Brain.Screen.printAt(20, 40,   "axis2:%d    ",(signed int)(*vcontroller).Axis2.position());
		Brain.Screen.printAt(20, 60,   "axis3:%d    ",(signed int)(*vcontroller).Axis3.position());
		Brain.Screen.printAt(20, 80,   "axis4:%d    ",(signed int)(*vcontroller).Axis4.position());
		Brain.Screen.printAt(180, 20,  "up:   %d",    (*vcontroller).ButtonUp.pressing());
		Brain.Screen.printAt(180, 40,  "down: %d",    (*vcontroller).ButtonDown.pressing());
		Brain.Screen.printAt(180, 60,  "left: %d",    (*vcontroller).ButtonLeft.pressing());
		Brain.Screen.printAt(180, 80,  "right:%d",    (*vcontroller).ButtonRight.pressing());
		Brain.Screen.printAt(260, 20,  "A:%d",        (*vcontroller).ButtonA.pressing());
		Brain.Screen.printAt(260, 40,  "B:%d",        (*vcontroller).ButtonB.pressing());
		Brain.Screen.printAt(260, 60,  "X:%d",        (*vcontroller).ButtonX.pressing());
		Brain.Screen.printAt(260, 80,  "Y:%d",        (*vcontroller).ButtonY.pressing());
		Brain.Screen.printAt(100, 100, "L1:%d",       (*vcontroller).ButtonL1.pressing());
		Brain.Screen.printAt(100, 120, "L2:%d",       (*vcontroller).ButtonL2.pressing());
		Brain.Screen.printAt(100, 140, "R1:%d",       (*vcontroller).ButtonR1.pressing());
		Brain.Screen.printAt(100, 180, "R2:%d",       (*vcontroller).ButtonR2.pressing());
	}
	*/
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
