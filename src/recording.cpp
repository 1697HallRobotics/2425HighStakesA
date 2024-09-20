#include "recording.h"

void start_recording(string filename, vex::brain* brain, vex::controller* controller, int maxSeconds)
{
    if (!brain->SDcard.isInserted())
    {
        brain->Screen.print("\nREC FAILED: NO SD CARD");
        return;
    }

    recording_output_stream.open(filename, ios::out | ios::trunc | ios::binary);

    if (!recording_output_stream.is_open())
    {
        brain->Screen.print("\nREC FAILED: STREAM OPEN FAILED");
        return;
    }

    recording_brain = brain;

    recording_controller = controller;

    max_recording_time = maxSeconds;

    recording_buffer = vector<ControllerData>();

    brain->Screen.print("\nRecording Started!");

    vex::thread thread = vex::thread(recording_thread);
    thread.detach();
}

void capture_controller()
{
    ControllerData data = {
        {(*recording_controller).Axis1.position(), (*recording_controller).Axis2.position(), (*recording_controller).Axis3.position(), (*recording_controller).Axis4.position()},
        {(*recording_controller).ButtonA.pressing(), (*recording_controller).ButtonB.pressing(), (*recording_controller).ButtonX.pressing(), (*recording_controller).ButtonY.pressing(), (*recording_controller).ButtonUp.pressing(), (*recording_controller).ButtonRight.pressing(), (*recording_controller).ButtonDown.pressing(), (*recording_controller).ButtonLeft.pressing(), (*recording_controller).ButtonL1.pressing(), (*recording_controller).ButtonL2.pressing(), (*recording_controller).ButtonR1.pressing(), (*recording_controller).ButtonR2.pressing()}
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

void recording_thread(void)
{
    auto invFpsLimit = chrono::duration_cast<chrono::high_resolution_clock::duration>(chrono::duration<double>{1.0/60.0});
    auto m_BeginFrame = chrono::high_resolution_clock::now();
    auto m_RecordingStartFrame = chrono::time_point_cast<chrono::seconds>(m_BeginFrame);
    auto m_EndFrame = m_BeginFrame + invFpsLimit;
    auto m_DeltaTime = chrono::high_resolution_clock::now() - m_BeginFrame;
    unsigned int frame_count_per_second = 0;
    auto prev_time_in_nanos = chrono::time_point_cast<chrono::seconds>(m_EndFrame);

    unsigned long long estimatedBytes = 0;

    while (true)
    {
        // Save the data to the recording buffer
        recording_brain->Screen.printAt(0, 40, "%lu controller captures", recording_buffer.size());
        capture_controller();

        // This part is just measuring if we're keeping the frame rate.
        // It is not necessary to keep the frame rate.
        auto now = chrono::high_resolution_clock::now();
        auto time_in_nanos = chrono::time_point_cast<chrono::seconds>(now);
        auto time_in_seconds = chrono::time_point_cast<chrono::seconds>(now);
        ++frame_count_per_second;
        if (time_in_nanos > prev_time_in_nanos)
        {
            // Flush the data to the file system to save RAM
            recording_brain->Screen.clearScreen();
            recording_brain->Screen.printAt(0, 20, "%lu", time_in_seconds.time_since_epoch().count() - m_RecordingStartFrame.time_since_epoch().count());
            if (time_in_seconds.time_since_epoch().count() - m_RecordingStartFrame.time_since_epoch().count() >= max_recording_time) break;
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

void begin_playback(string filename, vex::brain* brain)
{
    recording_brain = brain;
    //vex::thread thread = vex::thread(playback_thread);
    //thread.detach();
}