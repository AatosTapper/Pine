#pragma once

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <functional>
#include <cassert>
#include <iostream>
#include <optional>
#include <chrono>
#include <numeric>

#ifndef _SOL_INCLUDED
#define _SOL_INCLUDED
    #define SOL_SAFE_USERTYPE 1
    #define SOL_ALL_SAFETIES_ON 1
    #define SOL_EXCEPTIONS_ALWAYS_UNSAFE 1
    #include <sol/sol.hpp>
#endif