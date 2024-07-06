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
    float sx = 1.0;
    float sy = 1.0;

    void set_pos(double _x, double _y) { x = _x; y = _y; }
    void set_size(float _x, float _y) { sx = _x; sy = _y; }
};

/// Define a general script file that is ran when called
struct Script {
    Script() = default;
    Script(std::string str) : m_src(str) {}

    void set_src(std::string str) { m_src = str; }
    void run(sol::state &lua) { ScriptEngine::run_script(lua, SCRIPT() + m_src); }

private:
    std::string m_src;
};

/// Define custom functions for different object lifetime events
struct CustomBehaviour {
    void set_on_attach(sol::function func) { m_on_attach = func; }
    void set_on_update(sol::function func) { m_on_update = func; }
    void set_on_remove(sol::function func) { m_on_remove = func; }

    void call_on_attach() { m_on_attach(); }
    void call_on_update() { m_on_update(); }
    void call_on_remove() { m_on_remove(); }

private:
    sol::function m_on_attach;
    sol::function m_on_update;
    sol::function m_on_remove;
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


