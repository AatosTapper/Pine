#pragma once

#include "pch.h"
#include <iomanip>

/// @brief Put at the top of a scope to time it
#define PINE_CORE_PROFILE(message) Profiler p(message)

class Profiler {
public:
    Profiler(const char *message) : 
        m_start(std::chrono::high_resolution_clock::now()),
        m_message(message) {}
    ~Profiler() {
        const auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start).count();

        std::ios_base::fmtflags original_flags = std::cout.flags();
        std::streamsize original_precision = std::cout.precision();

        std::cout << std::fixed << std::setprecision(2);
        if (duration > 1'000'000) {
            std::cout << m_message << " - Duration: " << duration / 1'000'000.0 << " milliseconds\n";
        } else if (duration > 1'000) {
            std::cout << m_message << " - Duration: " << duration / 1'000.0f << " microseconds\n";
        } else {
            std::cout << m_message << " - Duration: " << duration << " nanoseconds\n";
        }

        std::cout.flags(original_flags);
        std::cout.precision(original_precision);
    }

private:
    std::chrono::steady_clock::time_point m_start;
    const char *m_message;
};