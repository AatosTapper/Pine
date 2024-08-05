#include "TickRateScaling.h"

#include "pch.h"

#include <deque>

static std::deque<double> dt_buffer;
constexpr static size_t buffer_window = 50;
constexpr static size_t start_wait = 5;

void accumulate_dt_buffer(double dt) {
    if (dt_buffer.size() >= buffer_window) [[likely]] {
        dt_buffer.pop_front();
    }
    dt_buffer.push_back(dt);
}

double manage_tick_rate(uint32_t &current_tick_rate, const uint32_t tick_rate_target, const uint32_t step) {
    if (dt_buffer.size() < start_wait) [[unlikely]] { // stabilize the first frames
        return 1.0 / tick_rate_target; 
    }

    constexpr double bias = 8.0;
    uint32_t rate_minus = 0;
    
    double average = std::reduce(dt_buffer.begin(), dt_buffer.end()) / dt_buffer.size();
    while (average >= (1.0 / (((double)tick_rate_target - (double)rate_minus - bias)))) {
        rate_minus += step;
        if (rate_minus >= tick_rate_target - step) break;
    }
    current_tick_rate = std::max(tick_rate_target - rate_minus, step);
    if (rate_minus > 0) {
        std::cout << "Tick rate lowered: " << current_tick_rate << "\n";
    }
    return 1.0 / current_tick_rate;
}