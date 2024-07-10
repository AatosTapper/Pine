#pragma once

#include "pch.h"
#include "config.h"
#include "SceneParser.h"
#include "ScriptEngine.h"
#include "rendering/QuadMesh.h"
#include "rendering/Texture.h"

class Renderer;
namespace component {

///
/// @brief User definable handle for an entity, name doesn't need to be unique
///
struct Tag {
    Tag() = default;
    Tag(std::string _name) : name(_name) {}
    std::string name;
};

///
/// @brief Entity transforms in world space
///
struct Transform {
    double x = 0.0;
    double y = 0.0;
    float sx = 1.0;
    float sy = 1.0;

    void set_pos(double _x, double _y) { x = _x; y = _y; }
    void set_scale(float _x, float _y) { sx = _x; sy = _y; }

    glm::mat4 get_matrix() const;
    operator glm::mat4() const { return get_matrix(); };
};

///
/// @brief Define any number of general script files that can be run whenever
///
struct Script {
    using id_t = uint32_t;
    Script() = default;
    Script(std::string str);

    id_t push_script(std::string str);
    void run(sol::state &lua, id_t id = 0);

private:
    std::vector<std::string> m_scripts;
};

///
/// @brief Define custom update and destroy functions
///
struct CustomBehaviour {
    CustomBehaviour() = default;
    CustomBehaviour(sol::function f);
    ~CustomBehaviour();

    void set_on_update(sol::function func);
    void set_on_remove(sol::function func);

    void call_on_update() const;

private:
    sol::function m_on_update;
    sol::function m_on_remove;
};

///
/// @brief General image component for drawing
///
struct Sprite {
    friend class ::Renderer;
    Sprite() = default;
    Sprite(std::string path);
    void set_texture(std::string path);

private:
    Texture *m_img = nullptr;
};

///
/// @brief Store any data within a lua table
///
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


