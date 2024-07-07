#pragma once

#include "pch.h"
#include "config.h"
#include "SceneParser.h"
#include "ScriptEngine.h"
#include "rendering/QuadMesh.h"
#include "rendering/TextureStorage.h"

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

    glm::mat4 get_matrix() const;
    operator glm::mat4() const { return get_matrix(); };
};

/// @brief Define a general script file that is ran when called
struct Script {
    Script() = default;
    Script(std::string str) : m_src(str) {}

    void set_src(std::string str) { m_src = str; }
    void run(sol::state &lua) { assert(!m_src.empty()); ScriptEngine::run_script(lua, SCRIPT() + m_src); }

private:
    std::string m_src;
};

/// @brief Define custom functions for different object lifetime events
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

struct Sprite {
    Sprite() {}
    Sprite(std::string path) : img(std::make_shared<Texture>(path)), temp_path(path) {
        img->filter_nearest();
    }

    std::shared_ptr<Texture> img = nullptr;
    QuadMesh mesh;
    TextureStorage::tex_id texture = 0;
    // used before knowing the texture storage
    std::string temp_path;
};

/// @brief Store any data via a lua table with an entity
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


