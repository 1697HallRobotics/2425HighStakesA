#include "recording.h"

void start_recording(string filename, vex::brain *brain, vex::controller *controller, int maxSeconds)
{
    if (!brain->SDcard.isInserted())
    {
        brain->Screen.printAt(30, 40, "\nREC FAILED: NO SD CARD");
        return;
    }

    if (!controller->installed())
    {
        brain->Screen.printAt(30, 40, "\nREC FAILED: CONTROLLER NOT INSTALLED");
        return;
    }

    recording_output_stream.open(filename, ios::out | ios::trunc | ios::binary);

    if (!recording_output_stream.is_open())
    {
        brain->Screen.printAt(30, 40, "\nREC FAILED: STREAM OPEN FAILED");
        return;
    }

    recording_brain = brain;

    recording_controller = controller;

    max_recording_time = maxSeconds;

    recording_buffer = vector<ControllerData>();

    recording_output_stream << (unsigned char)maxSeconds;

    brain->Screen.printAt(30, 40, "RECORDING STARTED");

    vex::thread thread = vex::thread(recording_thread);
    thread.detach();
}

void capture_controller()
{
    ControllerData data = {
        {(signed char)(*recording_controller).Axis1.position(), (signed char)(*recording_controller).Axis2.position(), (signed char)(*recording_controller).Axis3.position(), (signed char)(*recording_controller).Axis4.position()},
        {(signed char)(*recording_controller).ButtonA.pressing(), (signed char)(*recording_controller).ButtonB.pressing(), (signed char)(*recording_controller).ButtonX.pressing(), (signed char)(*recording_controller).ButtonY.pressing(), (signed char)(*recording_controller).ButtonUp.pressing(), (signed char)(*recording_controller).ButtonRight.pressing(), (signed char)(*recording_controller).ButtonDown.pressing(), (signed char)(*recording_controller).ButtonLeft.pressing(), (signed char)(*recording_controller).ButtonL1.pressing(), (signed char)(*recording_controller).ButtonL2.pressing(), (signed char)(*recording_controller).ButtonR1.pressing(), (signed char)(*recording_controller).ButtonR2.pressing()}};

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

void recording_thread(void)
{

    recording_brain->Screen.printAt(30, 80, "THREAD1");
    auto invFpsLimit = chrono::duration_cast<chrono::high_resolution_clock::duration>(chrono::duration<double>{1.0 / 60.0});
    auto m_BeginFrame = chrono::high_resolution_clock::now();
    auto m_RecordingStartFrame = chrono::time_point_cast<chrono::seconds>(m_BeginFrame);
    auto m_EndFrame = m_BeginFrame + invFpsLimit;
    auto m_DeltaTime = chrono::high_resolution_clock::now() - m_BeginFrame;
    unsigned int frame_count_per_second = 0;
    auto prev_time_in_nanos = chrono::time_point_cast<chrono::seconds>(m_EndFrame);

    unsigned long long estimatedBytes = 0;

    recording_brain->Screen.printAt(30, 120, "THREAD2");
//work now or else
    while (true)
    {
        recording_brain->Screen.printAt(30, 160, "THREAD3");
        // Save the data to the recording buffer
        recording_brain->Screen.printAt(0, 40, "%lu controller captures", recording_buffer.size());
        capture_controller();

        // This part is just measuring if we're keeping the frame rate.
        // It is not necessary to keep the frame rate.
        auto now = chrono::high_resolution_clock::now();
        auto time_in_nanos = chrono::time_point_cast<chrono::seconds>(now);
        ++frame_count_per_second;
        if (time_in_nanos > prev_time_in_nanos)
        {
            // Flush the data to the file system to save RAM
            recording_brain->Screen.clearScreen();
            recording_brain->Screen.printAt(0, 20, "%lu", time_in_nanos.time_since_epoch().count() - m_RecordingStartFrame.time_since_epoch().count());
            if (time_in_nanos.time_since_epoch().count() - m_RecordingStartFrame.time_since_epoch().count() >= max_recording_time)
                break;
            flush_recording_buffer();
            frame_count_per_second = 0;
            prev_time_in_nanos = time_in_nanos;
            estimatedBytes += 12ull;
        }

        // This part keeps the frame rate.
        vex::this_thread::sleep_until(m_EndFrame);
        m_DeltaTime = chrono::high_resolution_clock::now() - m_BeginFrame;
        m_BeginFrame = m_EndFrame;
        m_EndFrame = m_BeginFrame + invFpsLimit;
    }

    stop_recording();
}

void stop_recording()
{
    recording_output_stream.close();
}

virtual_controller *begin_playback(string filename, vex::brain *brain)
{
    playback_controller = new virtual_controller();

    recording_brain = brain;

    if (!brain->SDcard.isInserted())
    {
        brain->Screen.printAt(30, 40, "\nPLAYBACK FAILED: NO SD CARD");
        return nullptr;
    }

    ifstream stream;
    stream.open(filename, ios::in | ios::binary);

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

    vex::thread thread(playback_thread);
    thread.detach();

    return playback_controller;
}

void playback_thread(void)
{
    vex::this_thread::sleep_until(chrono::time_point_cast<chrono::seconds>(chrono::high_resolution_clock::now()));
    auto invFpsLimit = chrono::duration_cast<chrono::high_resolution_clock::duration>(chrono::duration<double>{1.0 / 60.0});
    auto m_BeginFrame = chrono::high_resolution_clock::now();
    auto m_EndFrame = m_BeginFrame + invFpsLimit;
    auto m_DeltaTime = chrono::high_resolution_clock::now() - m_BeginFrame;

    recording_brain->Screen.printAt(30, 120, "THREAD2");

    while (true)
    {
        recording_brain->Screen.printAt(30, 160, "THREAD3");
        if (playback_buffer.size() == 0) {
            recording_brain->Screen.printAt(30, 200, "END");
            break;
        }
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

        // This part keeps the tick rate.
        vex::this_thread::sleep_until(m_EndFrame);
        m_DeltaTime = chrono::high_resolution_clock::now() - m_BeginFrame;
        m_BeginFrame = m_EndFrame;
        m_EndFrame = m_BeginFrame + invFpsLimit;
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
