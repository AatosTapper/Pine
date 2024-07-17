#pragma once

#include <stdint.h>

void accumulate_dt_buffer(double dt);

double manage_tick_rate(uint32_t &current_tick_rate, const uint32_t tick_rate_target, const uint32_t step);