#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "v5.h"
#include "v5_vcs.h"

using namespace std;

struct ControllerData
{
    int axis[4];

    int digital[12];
};

string active_recording_filename;

vector<ControllerData> recording_buffer;

vector<ControllerData> playback_buffer;