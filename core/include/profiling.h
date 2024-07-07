#pragma once

#include "pch.h"

#define TRACK_PERFORMANCE(message, code) \
    do { \
        auto start = std::chrono::high_resolution_clock::now(); \
        code; \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(); \
        std::cout << message << " - Duration: " << duration << " nanoseconds\n"; \
    } while (0)