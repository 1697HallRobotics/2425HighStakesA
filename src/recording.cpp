#include "recording.h"

void start_recording(const string &filename, int maxSeconds)
{
    if (!usd::is_installed())
    {
        lcd::set_text(1, "REC FAILED: NO USD");
        return;
    }

    if (!controller_is_connected(E_CONTROLLER_MASTER))
    {
        lcd::set_text(1, "REC FAILED: NO CONTROLLER");
        return;
    }

    recording_output_stream.open("/usd/" + filename + ".vrf", ios::binary);

    if (!recording_output_stream.is_open())
    {
        lcd::set_text(1, "REC FAILED: OFSTREAM BAD");
        return;
    }

    max_recording_time = maxSeconds;

    recording_buffer = vector<ControllerData>();

    recording_output_stream << (unsigned char)maxSeconds;

    rtos::Task recording_task(recording_thread, nullptr, TASK_PRIORITY_MAX);
}

void capture_controller()
{
    ControllerData data = {
        {(signed char)(controller_get_analog(E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_RIGHT_X)),
         (signed char)(controller_get_analog(E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_RIGHT_Y)),
         (signed char)(controller_get_analog(E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_LEFT_Y)),
         (signed char)(controller_get_analog(E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_LEFT_X))},
        {(signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_A),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_B),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_X),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_Y),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_UP),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_RIGHT),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_DOWN),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_LEFT),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_L1),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_L2),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_R1),
         (signed char)controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_R2)}};

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
    pros::lcd::set_text(2, "THREAD STARTED");

    uint32_t captureDelay = 15; // 15ms per capture

    uint32_t m_BeginFrame = millis();

    uint32_t frame_count_per_second = 0;

    unsigned long long estimatedBytes = 0;

    uint32_t prev_millis = m_BeginFrame + captureDelay * 67;

    lcd::print(3, "%lu controller captures", recording_buffer.size());
    lcd::print(4, "%lu captures flushed to disk", frame_count_per_second);
    lcd::print(5, "flushed %lu times", estimatedBytes);

    while (true)
    {
        // Save the data to the recording buffer
        lcd::print(3, "%lu controller captures", recording_buffer.size());
        capture_controller();

        uint32_t now = millis();
        ++frame_count_per_second;
        if (now > prev_millis)
        {
            // Flush the data to the file system to save RAM
            lcd::print(4, "%lu captures flushed to disk", frame_count_per_second);

            if (now >= m_BeginFrame + max_recording_time * 1000)
                break;
            flush_recording_buffer();
            frame_count_per_second = 0;
            prev_millis = now + captureDelay * 67;
            estimatedBytes++;
            lcd::print(5, "flushed %lu times", estimatedBytes);
        }

        // This part keeps the frame rate.
        task_delay_until(&now, captureDelay);
    }

    stop_recording();
}

virtual_controller *begin_playback(string filename)
{
    playback_controller = new virtual_controller();

    if (!usd::is_installed())
    {
        lcd::set_text(1, "PLAYBACK FAILED: NO USD");
        return nullptr;
    }

    ifstream stream;
    stream.open("/usd/" + filename + ".vrf", ios::binary);

    char *lengthData = new char[1];
    stream.read(lengthData, sizeof(char));
    unsigned char recording_length = lengthData[0];

    while (!stream.eof())
    {
        char *raw = new char[16];
        stream.read(raw, 16);
        ControllerData data = {
            {(signed char)raw[0], (signed char)raw[1], (signed char)raw[2], (signed char)raw[3]},
            {(signed char)raw[4], (signed char)raw[5], (signed char)raw[6], (signed char)raw[7], (signed char)raw[8], (signed char)raw[9], (signed char)raw[10], (signed char)raw[11], (signed char)raw[12], (signed char)raw[13], (signed char)raw[14], (signed char)raw[15]}};
        playback_buffer.push_back(data);
    }

    rtos::Task playback_task(playback_thread, nullptr, TASK_PRIORITY_MAX);

    return playback_controller;
}

void playback_thread(void *param)
{
    uint32_t playbackDelay = 15; // 15ms per capture
    uint32_t m_BeginFrame = millis();

    while (1)
    {
        if (playback_buffer.size() == 0)
        {
            lcd::set_text(7, "END");
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
            playback_controller->copy_old();
            break;
        }

        playback_controller->copy_old();

        // Update the virtual controller to the current controller capture
        ControllerData data = playback_buffer[0];
        playback_buffer.pop_front();

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

        task_delay_until(&m_BeginFrame, playbackDelay);
        m_BeginFrame = millis();
    }
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

void stop_recording()
{
    flush_recording_buffer();
    recording_output_stream.close();
}