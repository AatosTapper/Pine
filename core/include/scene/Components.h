#pragma once

#include "pch.h"
#include "config.h"
#include "SceneParser.h"
#include "ScriptEngine.h"

// REMEMBER TO REGISTER THE COMPONENT IN LuaScene.cpp

namespace component {

struct Tag {
    Tag() = default;
    Tag(std::string _name) : name(_name) {}
    std::string name;
};

struct Transform {
    double x = 0.0;
    double y = 0.0;
    double sx = 1.0;
    double sy = 1.0;
};

struct Script {
    Script() {
        std::cout << "Script constructor called without arguments. " << std::endl;
    }
    Script(std::string _src) : src(_src) {
        std::cout << "Script constructor called with arguments: " << src << std::endl;
    }
    std::string src;
};

struct Table {
    Table() = default;
    Table(sol::table _data) : data(_data) {}
    sol::table data{};
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


