#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <thread>
#include <sys/unistd.h>

#include "api.h"

using namespace std;
using namespace chrono;
using namespace pros;
using namespace pros::c;

struct ControllerData //Controller data
{
    signed char axis[4];

    signed char digital[12];
};

class virtual_controller_axis
{
public:
    signed char position_value = 0;
    signed int position();
};

class virtual_controller_digital
{
public:
    signed char pressing_value = 0;
    bool pressing();
};

class virtual_controller
{
public:
    virtual_controller_digital PrevButtonA, PrevButtonB, PrevButtonX, PrevButtonY, PrevButtonUp, PrevButtonDown, PrevButtonLeft, PrevButtonRight, PrevButtonL1, PrevButtonL2, PrevButtonR1, PrevButtonR2;

    virtual_controller_axis Axis1, Axis2, Axis3, Axis4;
    virtual_controller_digital ButtonA, ButtonB, ButtonX, ButtonY, ButtonUp, ButtonDown, ButtonLeft, ButtonRight, ButtonL1, ButtonL2, ButtonR1, ButtonR2;

    void copy_old();
    int32_t get_analog(controller_analog_e_t channel);
    int32_t get_digital(controller_digital_e_t button);
    int32_t get_digital_new_press(controller_digital_e_t button);
};

static string active_recording_filename;

static ofstream recording_output_stream;
static vector<ControllerData> recording_buffer;
static uint32_t max_recording_time;

static virtual_controller* playback_controller;
static deque<ControllerData> playback_buffer;

void start_recording(const string& filename, int maxSeconds);
void recording_thread(void* param);
void stop_recording();

virtual_controller* begin_playback(string filename);
void playback_thread(void* param);
