#include "recording.h"

void start_recording(const string filename, int length, Gps* gps)
{
    // check if the SD card is installed
    if (!usd::is_installed())
    {
        screen_print(E_TEXT_MEDIUM, 7, "REC FAILED: NO USD (ENXIO)");
        return;
    }

    // check if the controller is installed
    // note: this uses the pros::c method in order to not create an object
    // in a word, readability
    if (!controller_is_connected(E_CONTROLLER_MASTER))
    {
        screen_print(E_TEXT_MEDIUM, 7, "REC FAILED: NO CONTROLLER (ENXIO)");
        return;
    }

    // when doing SD card IO, you MUST begin with /usd/
    recording_output_stream.open("/usd/" + filename + ".vrf", ios::binary);

    // make sure that nothing went wrong when opening the file stream
    // if this errors, god save you
    if (!recording_output_stream.is_open() || recording_output_stream.bad())
    {
        screen_print(E_TEXT_MEDIUM, 7, "REC FAILED: BAD OFSTREAM (EIO)");
        return;
    }

    max_recording_time = length;

    recording_buffer = vector<ControllerData>();

    recording_output_stream << (unsigned char)length;

    /*
    if (gps == nullptr) {
        recording_output_stream << 0.0f;
        recording_output_stream << 0.0f;
    } else {
        recording_output_stream << gps->get_position_x();
        recording_output_stream << gps->get_position_y();
    }
    */

    stop_system = false;

    rtos::Task recording_task(recording_thread, nullptr, TASK_PRIORITY_MAX);
}

void capture_controller()
{
    ControllerData data = {
        {
        (int8_t) controller_get_analog (E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_RIGHT_X),
        (int8_t) controller_get_analog (E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_RIGHT_Y),
        (int8_t) controller_get_analog (E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_LEFT_Y),
        (int8_t) controller_get_analog (E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_LEFT_X)
        },
        {
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_A),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_B),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_X),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_Y),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_UP),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_RIGHT),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_DOWN),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_LEFT),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_L1),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_L2),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_R1),
        (int8_t) controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_R2)
        }
    };

    recording_buffer.push_back(data);
}

void flush_recording_buffer()
{
    for (size_t i = 0; i < recording_buffer.size(); i++)
    {
        ControllerData capture = recording_buffer[i];
        for (size_t i = 0; i < 4; i++)
        {
            recording_output_stream << (char)capture.axis[i];
        }

        for (size_t i = 0; i < 12; i++)
        {
            recording_output_stream << (char)capture.digital[i];
        }
    }

    recording_output_stream.flush();

    recording_buffer.clear();
}

void recording_thread(void *param)
{
    #if DEBUG == 1
    lcd::set_text(2, "THREAD STARTED");
    #endif

    uint32_t captureDelay = 5; // 5ms per capture

    uint32_t beginFrame = millis();

    uint32_t captureCount = 0;

    uint32_t prev_millis = beginFrame + captureDelay * 67;

    uint32_t now = millis();
#if DEBUG == 1
    lcd::print(3, "%lu controller captures", recording_buffer.size());
    lcd::print(4, "%lu captures flushed to disk %lu frames ago", captureCount, now - beginFrame);
#endif
    while (true)
    {
        if (stop_system) break;

        // Save the data to the recording buffer
        #if DEBUG == 1
        lcd::print(3, "%lu controller captures", recording_buffer.size());
        #endif
        capture_controller();

        now = millis();
        ++captureCount;
        if (now > prev_millis)
        {
            // Flush the data to the file system to save RAM
            #if DEBUG == 1
            lcd::print(4, "%lu captures flushed to disk %lu frames ago", captureCount, now - beginFrame);
            #endif

            if (now >= beginFrame + max_recording_time * 1000)
                break;
            flush_recording_buffer();
            captureCount = 0;
            prev_millis = now + captureDelay * 67;
        }

        // This part keeps the frame rate.
        task_delay_until(&beginFrame, captureDelay);
        beginFrame = millis();
    }

    stop_recording();
}

void stop_recording()
{
    if (stop_system) {
        stop_system = false;
        return;
    }
    stop_system = true;
    flush_recording_buffer();
    recording_output_stream.close();
}

PositionData get_position(string filename)
{

}

virtual_controller *begin_playback(string filename)
{
    // allocate the virtual controller
    playback_controller = new virtual_controller();

    // ensure we have something to read the data from
    if (!usd::is_installed())
    {
        #if DEBUG == 1
        lcd::set_text(1, "PLAYBACK FAILED: NO USD");
        #endif
        return nullptr;
    }

    ifstream stream;
    stream.open("/usd/" + filename + ".vrf", ios::binary);

    if (!stream.is_open() || stream.bad())
    {
        screen_print(E_TEXT_MEDIUM, 7, "PLAYBACK FAILED: BAD IFSTREAM (EIO)");
        return nullptr;
    }

    // read the length of the recording
    // currently unused, but we need to advance the stream by one at least
    char *lengthData = new char[1];
    stream.read(lengthData, sizeof(char));
    /*
    char* positionData = new char[2*sizeof(float)];
    stream.read(positionData, 2*sizeof(float));
    */
    unsigned char recording_length = lengthData[0];

    // read the rest of the file 16 bytes at a time
    while (!stream.eof())
    {
        char *raw = new char[16];
        stream.read(raw, 16);
        ControllerData data = {
            {(signed char)raw[0], (signed char)raw[1], (signed char)raw[2], (signed char)raw[3]},
            {(signed char)raw[4], (signed char)raw[5], (signed char)raw[6], (signed char)raw[7], (signed char)raw[8], (signed char)raw[9], (signed char)raw[10], (signed char)raw[11], (signed char)raw[12], (signed char)raw[13], (signed char)raw[14], (signed char)raw[15]}};
        playback_buffer.push_back(data);
    }

    stop_system = false;

    rtos::Task playback_task(playback_thread, nullptr, TASK_PRIORITY_MAX);

    return playback_controller;
}

void playback_thread(void *param)
{
    uint32_t playbackDelay = 5; // 15ms per capture
    uint32_t beginFrame = millis();

    while (1)
    {
        if (playback_buffer.size() == 0 || stop_system) break;

        // copy the previous controller state for get_digital_get_new_press
        playback_controller->copy_old();

        // Update the virtual controller to the current controller capture
        ControllerData data = playback_buffer[0];
        playback_buffer.pop_front();

        // update controller state
        playback_controller->Axis1.position_value = (signed int)data.axis[0];
        playback_controller->Axis2.position_value = (signed int)data.axis[1];
        playback_controller->Axis3.position_value = (signed int)data.axis[2];
        playback_controller->Axis4.position_value = (signed int)data.axis[3];
        playback_controller->ButtonA.pressing_value = (signed int)data.digital[0];
        playback_controller->ButtonB.pressing_value = (signed int)data.digital[1];
        playback_controller->ButtonX.pressing_value = (signed int)data.digital[2];
        playback_controller->ButtonY.pressing_value = (signed int)data.digital[3];
        playback_controller->ButtonUp.pressing_value = (signed int)data.digital[4];
        playback_controller->ButtonRight.pressing_value = (signed int)data.digital[5];
        playback_controller->ButtonDown.pressing_value = (signed int)data.digital[6];
        playback_controller->ButtonLeft.pressing_value = (signed int)data.digital[7];
        playback_controller->ButtonL1.pressing_value = (signed int)data.digital[8];
        playback_controller->ButtonL2.pressing_value = (signed int)data.digital[9];
        playback_controller->ButtonR1.pressing_value = (signed int)data.digital[10];
        playback_controller->ButtonR2.pressing_value = (signed int)data.digital[11];

        // wait 5ms, accounting for execution delay (although if that is >1-2ms we might have a problem)
        task_delay_until(&beginFrame, playbackDelay);

        // update starting frame for delay
        beginFrame = millis();
    }

    stop_playback();
}

void stop_playback()
{
    if (stop_system) {
        stop_system = false;
        return;
    }
    stop_system = true;
    #if DEBUG == 1
    lcd::set_text(7, "END");
    #endif
    // zero out everything so the robot isn't moving to infinity
    playback_controller->Axis1.position_value = 0;
    playback_controller->Axis2.position_value = 0;
    playback_controller->Axis3.position_value = 0;
    playback_controller->Axis4.position_value = 0;
    playback_controller->ButtonA.pressing_value = 0;
    playback_controller->ButtonB.pressing_value = 0;
    playback_controller->ButtonX.pressing_value = 0;
    playback_controller->ButtonY.pressing_value = 0;
    playback_controller->ButtonUp.pressing_value = 0;
    playback_controller->ButtonRight.pressing_value = 0;
    playback_controller->ButtonDown.pressing_value = 0;
    playback_controller->ButtonLeft.pressing_value = 0;
    playback_controller->ButtonL1.pressing_value = 0;
    playback_controller->ButtonL2.pressing_value = 0;
    playback_controller->ButtonR1.pressing_value = 0;
    playback_controller->ButtonR2.pressing_value = 0;
    // also overwrite the old data to ensure nothing gets messed up
    playback_controller->copy_old();

    playback_buffer.clear();

    // free the allocated playback controller to prevent a memory leak when loading multiple recordings in one session
    delete playback_controller;
}

int virtual_controller_axis::position()
{
    return this->position_value;
}

bool virtual_controller_digital::pressing()
{
    return this->pressing_value;
}

int32_t virtual_controller::get_analog(controller_analog_e_t channel)
{
    switch (channel)
    {
    case E_CONTROLLER_ANALOG_RIGHT_X:
        return Axis1.position();
    case E_CONTROLLER_ANALOG_RIGHT_Y:
        return Axis2.position();
    case E_CONTROLLER_ANALOG_LEFT_Y:
        return Axis3.position();
    case E_CONTROLLER_ANALOG_LEFT_X:
        return Axis4.position();
    default:
        return 0;
    }
}

int32_t virtual_controller::get_digital(controller_digital_e_t button)
{
    switch (button)
    {
    case E_CONTROLLER_DIGITAL_A:
        return ButtonA.pressing();
    case E_CONTROLLER_DIGITAL_B:
        return ButtonB.pressing();
    case E_CONTROLLER_DIGITAL_X:
        return ButtonX.pressing();
    case E_CONTROLLER_DIGITAL_Y:
        return ButtonY.pressing();
    case E_CONTROLLER_DIGITAL_UP:
        return ButtonUp.pressing();
    case E_CONTROLLER_DIGITAL_DOWN:
        return ButtonDown.pressing();
    case E_CONTROLLER_DIGITAL_LEFT:
        return ButtonLeft.pressing();
    case E_CONTROLLER_DIGITAL_RIGHT:
        return ButtonRight.pressing();
    case E_CONTROLLER_DIGITAL_L1:
        return ButtonL1.pressing();
    case E_CONTROLLER_DIGITAL_L2:
        return ButtonL2.pressing();
    case E_CONTROLLER_DIGITAL_R1:
        return ButtonR1.pressing();
    case E_CONTROLLER_DIGITAL_R2:
        return ButtonR2.pressing();
    default:
        return 0;
    }
}

int32_t virtual_controller::get_digital_new_press(controller_digital_e_t button)
{
    switch (button)
    {
    case E_CONTROLLER_DIGITAL_A:
        return ButtonA.pressing() && !PrevButtonA.pressing();
    case E_CONTROLLER_DIGITAL_B:
        return ButtonB.pressing() && !PrevButtonB.pressing();
    case E_CONTROLLER_DIGITAL_X:
        return ButtonX.pressing() && !PrevButtonX.pressing();
    case E_CONTROLLER_DIGITAL_Y:
        return ButtonY.pressing() && !PrevButtonY.pressing();
    case E_CONTROLLER_DIGITAL_UP:
        return ButtonUp.pressing() && !PrevButtonUp.pressing();
    case E_CONTROLLER_DIGITAL_DOWN:
        return ButtonDown.pressing() && !PrevButtonDown.pressing();
    case E_CONTROLLER_DIGITAL_LEFT:
        return ButtonLeft.pressing() && !PrevButtonLeft.pressing();
    case E_CONTROLLER_DIGITAL_RIGHT:
        return ButtonRight.pressing() && !PrevButtonRight.pressing();
    case E_CONTROLLER_DIGITAL_L1:
        return ButtonL1.pressing() && !PrevButtonL1.pressing();
    case E_CONTROLLER_DIGITAL_L2:
        return ButtonL2.pressing() && !PrevButtonL2.pressing();
    case E_CONTROLLER_DIGITAL_R1:
        return ButtonR1.pressing() && !PrevButtonR1.pressing();
    case E_CONTROLLER_DIGITAL_R2:
        return ButtonR2.pressing() && !PrevButtonR2.pressing();
    default:
        return 0;
    }
}

void virtual_controller::copy_old()
{
    PrevButtonA     = ButtonA;
    PrevButtonB     = ButtonB;
    PrevButtonX     = ButtonX;
    PrevButtonY     = ButtonY;
    PrevButtonUp    = ButtonUp;
    PrevButtonRight = ButtonRight;
    PrevButtonDown  = ButtonDown;
    PrevButtonLeft  = ButtonLeft;
    PrevButtonL1    = ButtonL1;
    PrevButtonL2    = ButtonL2;
    PrevButtonR1    = ButtonR1;
    PrevButtonR2    = ButtonL2;
}