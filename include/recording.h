#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <chrono>

#include "v5.h"
#include "v5_vcs.h"

using namespace std;
using namespace chrono;

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
    virtual_controller_axis Axis1, Axis2, Axis3, Axis4;
    virtual_controller_digital ButtonA, ButtonB, ButtonX, ButtonY, ButtonUp, ButtonDown, ButtonLeft, ButtonRight, ButtonL1, ButtonL2, ButtonR1, ButtonR2;
};

static string active_recording_filename;

static vex::brain* recording_brain;
static vex::controller* recording_controller;
static ofstream recording_output_stream;
static vector<ControllerData> recording_buffer;
static int max_recording_time;

static virtual_controller* playback_controller;
static deque<ControllerData> playback_buffer;

void start_recording(string filename, vex::brain* brain, vex::controller* controller, int maxSeconds);
void recording_thread(void);
void stop_recording();

virtual_controller* begin_playback(string filename, vex::brain* brain);
void playback_thread(void);
