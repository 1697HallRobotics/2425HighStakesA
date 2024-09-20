#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

#include "v5.h"
#include "v5_vcs.h"

using namespace std;
using namespace chrono;

struct ControllerData //Controller data
{
    int axis[4];

    int digital[12];
};

class virtual_controller_axis
{
public:
    char position_value = 0;
    int position();
};

class virtual_controller_digital
{
public:
    char pressing_value = 0;
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
static vector<ControllerData> playback_buffer;

void start_recording(string filename, vex::brain* brain, vex::controller* controller, int maxSeconds);
void recording_thread(void);
void stop_recording();