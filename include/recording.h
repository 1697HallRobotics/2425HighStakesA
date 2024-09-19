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

static string active_recording_filename;

static vex::brain* recording_brain;
static vex::controller* recording_controller;
static ofstream recording_output_stream;
static vector<ControllerData> recording_buffer;
static int max_recording_time;

static ifstream* recording_playback_stream;
static vector<ControllerData> playback_buffer;

void start_recording(string filename, vex::brain* brain, vex::controller* controller, int maxSeconds);
void recording_thread(void);
void stop_recording();