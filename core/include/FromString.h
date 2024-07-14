#pragma once

#include "pch.h"

#include <sstream>

template <typename T> 
[[nodiscard]] inline constexpr T from_string(const std::string &str) {
    std::istringstream ss(str);
    T element;
    ss >> element;
    return element;
}