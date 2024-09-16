#include "recording.h"

void start_recording(string filename, vex::brain brain, ofstream &stream)
{
    if (!brain.SDcard.isInserted())
    {
        brain.Screen.print("\nREC FAILED: NO SD CARD");
        return;
    }

    stream.open(filename.c_str(), ios::out | ios::trunc);

    if (!stream.is_open())
    {
        brain.Screen.print("\nREC FAILED: STREAM OPEN FAILED");
        return;
    }

    active_recording_filename = filename;

    record_brain = &brain;

    brain.Screen.print("\nRecording Started!");
}

void capture_controller(vex::controller controller)
{
    ControllerData data = {
        {controller.Axis1.position(), controller.Axis2.position(), controller.Axis3.position(), controller.Axis4.position()},
        {controller.ButtonA.pressing(), controller.ButtonB.pressing(), controller.ButtonX.pressing(), controller.ButtonY.pressing(), controller.ButtonUp.pressing(), controller.ButtonRight.pressing(), controller.ButtonDown.pressing(), controller.ButtonLeft.pressing(), controller.ButtonL1.pressing(), controller.ButtonL2.pressing(), controller.ButtonR1.pressing(), controller.ButtonR2.pressing()}};

    recording_buffer.push_back(data);
}

void flush_recording_buffer(ofstream &stream, vex::brain brain)
{
    for (size_t i = 0; i < recording_buffer.size(); i++)
    {
        ControllerData capture = recording_buffer[i];
        for (size_t i = 0; i < 4; i++)
        {
            stream << (char)capture.axis[i];
        }

        for (size_t i = 0; i < 12; i++)
        {
            stream << (char)capture.digital[i];
        }
    }

    recording_buffer.clear();
}

void stop_recording(ofstream &stream, vex::brain brain)
{
    flush_recording_buffer(stream, brain);
    stream.close();
}

void begin_playback(string filename, vex::brain* brain)
{
    record_brain = brain;
    vex::thread thread = vex::thread(playback_thread);
    thread.detach();
}

void playback_thread(void)
{
    vex::this_thread::sleep_for(500);
    auto invFpsLimit = chrono::duration_cast<chrono::system_clock::duration>(chrono::duration<double>{1.0/60.0});
    auto m_BeginFrame = chrono::system_clock::now();
    auto m_EndFrame = m_BeginFrame + invFpsLimit;
    auto m_DeltaTime = chrono::system_clock::now() - m_BeginFrame;
    unsigned int frame_count_per_second = 0;
    auto prev_time_in_seconds = chrono::time_point_cast<chrono::seconds>(m_BeginFrame);

    unsigned long long estimatedBytes = 0;

    while (true)
    {
        // Save the data to the recording buffer
        record_brain->Screen.printAt(0, 40, "%lu frames per second", frame_count_per_second);

        // This part is just measuring if we're keeping the frame rate.
        // It is not necessary to keep the frame rate.
        auto time_in_seconds = chrono::time_point_cast<chrono::seconds>(chrono::system_clock::now());
        ++frame_count_per_second;
        if (time_in_seconds > prev_time_in_seconds)
        {
            // Flush the data to the file system to save RAM
            record_brain->Screen.clearScreen();
            record_brain->Screen.printAt(0, 20, "%lu bytes total (est.)", estimatedBytes);
            frame_count_per_second = 0;
            prev_time_in_seconds = time_in_seconds;
            estimatedBytes += 12ull;
        }

        // This part keeps the frame rate.
        vex::this_thread::sleep_until(m_EndFrame);
        m_DeltaTime = chrono::system_clock::now() - m_BeginFrame;
        m_BeginFrame = m_EndFrame;
        m_EndFrame = m_BeginFrame + invFpsLimit;
    }
    
}