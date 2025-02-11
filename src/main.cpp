// ☆*: .｡. o(≧▽≦)o .｡.:*☆
// 😊😊😊😊😊😊😊😊😊😊😊😊😊😁😁😁😁😁😁🙌🙌🙌🙌🙌
//ヾ(＠⌒ー⌒＠)ノ♪(´▽｀)( •̀ ω •́ )y(o゜▽゜)o☆(´･ω･`)?

#include "main.h"
#include "images.h"

int16_t xVelo = 0;
int16_t yVelo = 0;
uint64_t dvdTimer = 0;
//#define RECORD "auton_R_B"
//#define RLENGTH 15
#define SKILLS "auton_skills1"
//#define OVERRIDE_PLAYBACK "auton_R_B"

#define DRIVE()                                                                             \
uint8_t intakeSpinning = 0;                                                                 \
uint8_t sweeperState = 1;																	\
sweeperMotor.move_absolute(215, 40);														\
while (1)                                                                                   \
{                                                                                           \
	screen::print(TEXT_MEDIUM, 1, "X: %f", gps.get_position_x());							\
	screen::print(TEXT_MEDIUM, 2, "Y: %f", gps.get_position_y());							\
	screen::print(TEXT_MEDIUM, 3, "heaindg: %f", gps.get_heading());						\
	screen::print(TEXT_MEDIUM, 4, "Temp: BL(%lf) BR(%lf)", leftBackMotor.get_temperature(), rightBackMotor.get_temperature());\
	screen::print(TEXT_MEDIUM, 5, "Temp: FL(%lf) FR(%lf)", leftFrontMotor.get_temperature(), rightFrontMotor.get_temperature());\
	float turnPower = CONTROLLER.get_analog(ANALOG_RIGHT_X);                                \
	float forwardPower = CONTROLLER.get_analog(ANALOG_LEFT_Y);                              \
	if (fabsf(turnPower) <= deadzone)                                                       \
		turnPower = 0;                                                                      \
	if (fabsf(forwardPower) <= deadzone)                                                    \
		forwardPower = 0;                                                                   \
\
	if (turnPower != 0 || forwardPower != 0)                                            	\
	{                                                                                   	\
		rightMotors.move((turnPower - forwardPower) * speed);      				           	\
		leftMotors.move((turnPower + forwardPower) * speed);     			              	\
	}                                                                                   	\
	else                                                                                	\
	{                                                                                   	\
		rightMotors.brake();                                                            	\
		leftMotors.brake();                                                             	\
	}                                                                                   	\
\
	if (CONTROLLER.get_digital_new_press(DIGITAL_B))                                    	\
		clampPneumatics.toggle();                                                       	\
\
	if (CONTROLLER.get_digital(DIGITAL_R1)) {												\
		intakeMotor.move((CONTROLLER.get_digital(DIGITAL_R2) ? -127 : 127));                \
	} else {																				\
		intakeMotor.brake();                                                           		\
	}																						\
\
	if (CONTROLLER.get_digital_new_press(DIGITAL_L1))										\
	{																						\
		if (sweeperState) {																	\
			sweeperMotor.move_absolute(447, 40);											\
			sweeperState = 0;																\
		} else {																			\
			sweeperMotor.move_absolute(215, 40);											\
			sweeperState = 1;																\
		}																					\
	}																						\
\
	if (!clampPneumatics.is_extended()) controller.print(0, 0, "CLAMPED!!     "); 			\
	else controller.print(0, 0, "locked out asf");											\
	task_delay(5);                                                                      	\
\
	lv_timer_handler();                                                                 	\
}																							\

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
	
	if (battery::get_capacity() < 15) {
		if (++dvdTimer > 33)
		{
			lv_style_set_img_recolor(dvd_img->styles->style, LV_COLOR_MAKE(255, 0, 0));
		}
		else if (dvdTimer > 66)
		{
			dvdTimer = 0;
			lv_style_set_img_recolor(dvd_img->styles->style, LV_COLOR_MAKE(255, 255, 255));
		}
	}
}

void update_cat(lv_timer_t* timer) {
	lv_img_set_src(dvd_img, &(cat_gif[(dvdTimer) % 24]));
	dvdTimer++;
	lv_obj_invalidate(dvd_img);
}

char* autonFileName;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	//init_catgif();

	/*
	xVelo = rand() % 4;
	yVelo = rand() % 4;
	dvd_img = lv_img_create(lv_scr_act());
	lv_img_set_src(dvd_img, &dvd_logo);

	lv_style_t style1;
	lv_style_init(&style1);
	lv_style_set_img_recolor_opa(&style1, LV_OPA_COVER);
	lv_style_set_img_recolor(&style1, lv_color_white());
	lv_obj_add_style(dvd_img, &style1, LV_STATE_DEFAULT);
	*/
#ifndef RECORD
#ifndef SKILLS
	lv_obj_t* label = lv_label_create(lv_scr_act());
	lv_label_set_text(label, "Blue\nLeft side or Right side\nRed");
	lv_style_t style1;
	lv_style_init(&style1);
	lv_style_set_text_font(&style1, &lv_font_montserrat_24);
	lv_style_set_text_align(&style1, LV_TEXT_ALIGN_CENTER);
	lv_obj_add_style(label, &style1, LV_STATE_DEFAULT);
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	
	bool touched = false;
	bool touchCount = screen::touch_status().press_count;

	while (!touched)
	{
		if (screen::touch_status().press_count > touchCount) {
			touched = true;
			if (screen::touch_status().y <= 120) {
				autonFileName = (screen::touch_status().x <= 240 ? (char*)"auton_L_B" : (char*)"auton_R_B");
			} else {
				autonFileName = (screen::touch_status().x <= 240 ? (char*)"auton_L_R" : (char*)"auton_R_R");
			}
			lv_label_set_text(label, (string(autonFileName) + to_string(screen::touch_status().y)).c_str());
			delay(1000);
		}
		delay(5);
	}
	
	lv_obj_del(label);
#endif
#endif
	#ifdef SKILLS
	autonFileName = SKILLS;


	#endif

	// init and set up image
	//dvd_img = lv_img_create(lv_scr_act());
	//lv_img_set_src(dvd_img, &frame_00);
	//lv_obj_align(dvd_img, LV_ALIGN_CENTER, 0, 0);
	//lv_img_set_angle(dvd_img, 180);
	//lv_obj_set_size(dvd_img, 240, 240);
	//lv_img_set_zoom(dvd_img, 512);

	// create timer for gif frame swapping
	//lv_timer_create(update_cat, 120, NULL);

	// set up motors
	rightMotors.set_brake_mode_all(MotorBrake::brake);
	leftMotors.set_brake_mode_all(MotorBrake::brake);

	intakeMotor.set_brake_mode(MotorBrake::hold);
	rightMotors.set_zero_position_all(0);
	leftMotors.set_zero_position_all(0);
	intakeMotor.set_zero_position(0);
	sweeperMotor.set_zero_position(0);

	clampPneumatics.set_value(1);
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

void autonomous_cool()
{
	#ifndef OVERRIDE_PLAYBACK
	virtual_controller* vcontroller = begin_playback(autonFileName);
	#else
	virtual_controller* vcontroller = begin_playback(OVERRIDE_PLAYBACK);
	#endif
	if (vcontroller == nullptr)
		return;

	#define CONTROLLER (*vcontroller)

	DRIVE();
	
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
	//autonomous_cool();
	leftMotors.move(127);
	rightMotors.move(-127);
	delay(500);
	leftMotors.brake();
	rightMotors.brake();
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
#ifdef RECORD
	start_recording(RECORD, RLENGTH, nullptr);
#endif
	// this is used for autonomous to easily swap out the controller with the virtual controller
	#define CONTROLLER controller

	DRIVE();

	#undef CONTROLLER
}