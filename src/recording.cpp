#include "recording.h"

void start_recording(string filename, vex::brain brain, ofstream& stream) {
    if (!brain.SDcard.isInserted()) {
        brain.Screen.print("\nREC FAILED: NO SD CARD");
        return;
    }

    stream.open(filename.c_str(), ios::out | ios::trunc);

    if (!stream.is_open()) {
        brain.Screen.print("\nREC FAILED: STREAM OPEN FAILED");
        return;
    }

    active_recording_filename = filename;

    brain.Screen.print("\nRecording Started!");
}

void capture_controller(vex::controller controller) {
    ControllerData data = { 
    {controller.Axis1.position(), controller.Axis2.position(), controller.Axis3.position(), controller.Axis4.position()}, 
    {controller.ButtonA.pressing(), controller.ButtonB.pressing(), controller.ButtonX.pressing(), controller.ButtonY.pressing(), controller.ButtonUp.pressing(), controller.ButtonRight.pressing(), controller.ButtonDown.pressing(), controller.ButtonLeft.pressing(), controller.ButtonL1.pressing(), controller.ButtonL2.pressing(), controller.ButtonR1.pressing(), controller.ButtonR2.pressing()} };

    recording_buffer.push_back(data);
}

void flush_recording_buffer(ofstream& stream) {
    
}

void stop_recording(void) {

}

void begin_playback(string filename) {

}