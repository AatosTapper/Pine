#pragma once

#include "pch.h"
#include "config.h"
#include "Serialize.h"
#include "ScriptEngine.h"
#include "rendering/QuadMesh.h"
#include "rendering/Texture.h"

class Renderer;
class Entity;
namespace component {

///
/// @brief User definable handle for an entity, name doesn't need to be unique
/// @serializable
struct Tag : public Serializable {
    Tag() noexcept = default;
    Tag(std::string _name) noexcept;
    virtual ~Tag() override = default;

    std::string name;

    virtual void deserialize(NodeData &data) override;
    virtual NodeData serialize() const override;
};

///
/// @brief Entity transforms in world space
/// @serializable
struct Transform : public Serializable {
    double x = 0.0;
    double y = 0.0;
    float sx = 1.0; /// width (x scale)
    float sy = 1.0; /// height (y scale)
    float rr = 0.0; /// rotation in radians

    void set_pos(double _x, double _y) { x = _x; y = _y; }
    void set_scale(float _x, float _y) { sx = _x; sy = _y; }

    glm::mat4 get_matrix() const;
    operator glm::mat4() const { return get_matrix(); };

    virtual ~Transform() override = default;
    virtual void deserialize(NodeData &data) override;
    virtual NodeData serialize() const override;
};

///
/// @brief Define any number of general script files that can be run whenever
/// @serializable
struct Script : public Serializable {
    using id_t = uint32_t;
    Script() noexcept = default;
    Script(std::string str) noexcept;
    virtual ~Script() override = default;

    id_t push_script(std::string str);
    void run(id_t id = 0) const;
    void run_all() const;
    
    virtual void deserialize(NodeData &data) override;
    virtual NodeData serialize() const override;

private:
    std::vector<std::string> m_scripts;
};

///
/// @brief Define custom update and destroy scripts
/// @serializable
struct CustomBehaviour : public Serializable {
    CustomBehaviour() noexcept = default;
    CustomBehaviour(std::string path) noexcept;
    virtual ~CustomBehaviour() override;

    void set_on_update(std::string path);
    void set_on_remove(std::string path);

    void call_on_update() const;

    virtual void deserialize(NodeData &data) override;
    virtual NodeData serialize() const override;

private:
    std::string m_on_update;
    std::string m_on_remove;
};

///
/// @brief General image component for drawing
/// @serializable
struct Sprite : public Serializable {
    friend class ::Renderer;
    Sprite() noexcept = default;
    Sprite(std::string path) noexcept;
    virtual ~Sprite() = default;

    void set_texture(std::string path);

    virtual void deserialize(NodeData &data) override;
    virtual NodeData serialize() const override;
private:
    Texture *m_img = nullptr;
    std::shared_ptr<std::string> m_save_string;
};

///
/// @brief Store any data within a lua table
/// serializable (but noticably slow, >50 microseconds)
struct Table : public Serializable {
    Table() noexcept = default;
    Table(sol::table _data) noexcept;
    virtual ~Table() override = default;

    sol::table table{};
    
    virtual void deserialize(NodeData &data) override;
    virtual NodeData serialize() const override;
};

// Example on implementing serialization
struct TestSerdeComp : public Serializable {
    virtual void deserialize(NodeData &data) override {
        // pack the node data into the C++ variables
        (void)data;
    }
    virtual NodeData serialize() const override {
        // pack the C++ variables into the node data
        return NodeData{ .type=NodeType::Undefined };
    }
};

} // namespace component


