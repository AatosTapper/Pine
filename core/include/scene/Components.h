#pragma once

#include "pch.h"
#include "config.h"
#include "SceneParser.h"

// REMEMBER TO REGISTER THE COMPONENT IN LuaScene.cpp

namespace component {

struct Tag {
    std::string name;
};

struct Transform {
    double x = 0.0;
    double y = 0.0;
    double sx = 1.0;
    double sy = 1.0;
};

struct Script {
    std::string src;
};

struct VarInt {
    int var;
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


