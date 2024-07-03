#pragma once

#include "pch.h"
#include "config.h"

namespace component {

struct Tag {
    std::string name;
};

struct Transform {
    glm::mat4 transform{};
};

} // namespace component


