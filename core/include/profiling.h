#pragma once

#include "pch.h"

/// @brief Create this on the stack at the start of a scope you want to time
class Profiler {
public:
    Profiler(const char *message) : 
        m_start(std::chrono::high_resolution_clock::now()),
        m_end(std::chrono::high_resolution_clock::now()),
        m_message(message) {}
    ~Profiler() {
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_start).count();
        std::cout << m_message << " - Duration: " << duration << " nanoseconds\n";
    }

private:
    std::chrono::steady_clock::time_point m_start;
    std::chrono::steady_clock::time_point m_end;
    const char *m_message;
};