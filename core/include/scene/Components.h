#pragma once

#include "pch.h"
#include "config.h"
#include "SceneParser.h"

namespace component {

struct Tag {
    std::string name;
};

struct Transform {
    glm::mat4 transform{};
};

// Example on implementing serialization
struct TestSerdeComp : public Serializable {
    virtual void deserialize(NodeData &data) override {
        // pack the node data into the C++ variables
        (void)data;
    }
    virtual NodeData serialize() override {
        // pack the C++ variables into the node data
        return NodeData{ .type=NodeType::Undefined };
    }
};

} // namespace component


