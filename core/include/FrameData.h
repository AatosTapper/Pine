#pragma once

#include "config.h"
#include "pch.h"

struct FrameData {
    FrameData(double _updates_per_sec) noexcept {
        frame_counter = 0;
        update_counter = 0;
        frametime_accumulator = 0.0;
        last_update = glfwGetTime();
        update_time = 1.0 / _updates_per_sec;
        fps_log_accumulator = 0.0;
        updates_per_sec = static_cast<uint32_t>(_updates_per_sec);
    };

    double frametime_accumulator = 0.0;
    double last_update = 0.0;
    double update_time = 0.0;
    double fps_log_accumulator = 0.0;

    uint32_t frame_counter = 0u;
    uint32_t update_counter = 0u;
    uint32_t updates_per_sec = 0u;

    void update_frame_data() {
        const double frame_start_time = glfwGetTime();
        const double delta_time = frame_start_time - last_update;
        last_update = frame_start_time;
        frametime_accumulator += delta_time;
        fps_log_accumulator += delta_time;
        frame_counter++;
    }

    void log_frametime() {
        std::cout << "Frametime (capped): " 
            << static_cast<uint32_t>(fps_log_accumulator * 1000000.0) / 1000.0f << "ms = " 
            << static_cast<uint32_t>(std::round(1.0 / fps_log_accumulator)) << " FPS\n";
    }

    void log_frame() {
        if (update_counter == updates_per_sec) [[unlikely]] {
            fps_log_accumulator /= static_cast<double>(frame_counter);
            log_frametime();
            fps_log_accumulator = 0.0;
            update_counter = 0u;
            frame_counter = 0u;
        }
    }
};