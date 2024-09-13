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

struct ControllerData
{
    int axis[4];

    int digital[12];
};
struct EventData
{
    bool axis[4];

    bool digital[12];
};

static string active_recording_filename;

static vex::brain* record_brain;

static vector<ControllerData> recording_buffer;

static vector<ControllerData> playback_buffer;

void begin_playback(string filename, vex::brain* brain);
void playback_thread(void);