#pragma once

#include "pch.h"
#include "config.h"
#include "Serialize.h"
#include "ScriptEngine.h"
#include "rendering/QuadMesh.h"
#include "rendering/Texture.h"

void print_component_sizes();

class Renderer;
class Entity;
struct CollisionData;
namespace component {

///
/// @brief User definable handle for an entity, name doesn't need to be unique
/// @serializable
struct Tag : public Serializable {
    friend class ::Entity;

    Tag() noexcept = default;
    Tag(std::string _name) noexcept;
    virtual ~Tag() override = default;

    std::string name;

    virtual void deserialize(SceneNodeData &data) override;
    virtual SceneNodeData serialize() const override;

private:
    std::shared_ptr<Entity> m_parent; // behind pointer to not pollute the cache as much
};

///
/// @brief Entity transforms in world space
/// @serializable
struct Transform : public Serializable {
    friend class ::Entity;

    double x = 0.0;
    double y = 0.0;
    double x_0 = 0.0; // last
    double y_0 = 0.0; // last
    double x_interpolated = 0.0;
    double y_interpolated = 0.0;

    float sx = 1.0; /// width (x scale)
    float sy = 1.0; /// height (y scale)
    float sx_0 = 1.0;
    float sy_0 = 1.0;
    float sx_interpolated = 1.0;
    float sy_interpolated = 1.0;

    float rr = 0.0; /// rotation in radians
    float rr_0 = 0.0;
    float rr_interpolated = 0.0;

    void set_pos(double _x, double _y) { x = _x; y = _y; }
    void set_scale(float _x, float _y) { sx = _x; sy = _y; }

    glm::mat4 get_matrix() const;
    operator glm::mat4() const { return get_matrix(); };

    void save_last_position();
    void interpolate(float alpha);

    virtual ~Transform() override = default;
    virtual void deserialize(SceneNodeData &data) override;
    virtual SceneNodeData serialize() const override;

private:
    std::shared_ptr<Entity> m_parent;
};

///
/// @brief Define any number of general script files that can be run whenever
/// @serializable
struct Script : public Serializable {
    friend class ::Entity;

    using id_t = uint32_t;
    Script() noexcept = default;
    Script(std::string str) noexcept;
    virtual ~Script() override = default;

    id_t push_script(std::string str);
    void run(id_t id = 0) const;
    void run_all() const;
    
    virtual void deserialize(SceneNodeData &data) override;
    virtual SceneNodeData serialize() const override;

private:
    std::vector<std::string> m_scripts;
    std::shared_ptr<Entity> m_parent;
};

///
/// @brief Define custom update and destroy scripts
/// @serializable
struct CustomBehavior : public Serializable {
    friend class ::Entity;

    CustomBehavior() noexcept = default;
    CustomBehavior(std::string path) noexcept;
    virtual ~CustomBehavior() override;

    void set_on_update(std::string path);
    void set_on_remove(std::string path);

    std::string on_update;
    std::string on_remove;

    virtual void deserialize(SceneNodeData &data) override;
    virtual SceneNodeData serialize() const override;

private:
    std::shared_ptr<Entity> m_parent;
};

///
/// @brief General image component for drawing
/// @serializable
struct Sprite : public Serializable {
    friend class ::Entity;

    Sprite() noexcept = default;
    Sprite(std::string path) noexcept;
    virtual ~Sprite() = default;

    Texture *img = nullptr;
    /// @brief z offset to help with ordering the sprites
    float render_layer = 0.0f;

    void set_texture(std::string path);

    virtual void deserialize(SceneNodeData &data) override;
    virtual SceneNodeData serialize() const override;
private:
    std::shared_ptr<std::string> m_save_string;
    std::shared_ptr<Entity> m_parent;
};

///
/// @brief Store any data within a lua table
/// @serializable
struct Table : public Serializable {
    friend class ::Entity;

    Table() noexcept = default;
    Table(sol::table _data) noexcept;
    virtual ~Table() override = default;

    sol::table table{};
    
    virtual void deserialize(SceneNodeData &data) override;
    virtual SceneNodeData serialize() const override;

private:
    std::shared_ptr<Entity> m_parent;
};

///
/// @brief Define custom flag strings to sort components
/// @Serializable
struct StateFlags : public Serializable {
    friend class ::Entity;

    StateFlags() noexcept = default;
    StateFlags(std::vector<std::string> flags) noexcept;
    virtual ~StateFlags() override = default;

    void set_flags(std::vector<std::string> flags);
    bool has_flags(std::vector<std::string> flags);
    void remove_flags(std::vector<std::string> flags);

    virtual void deserialize(SceneNodeData &data) override;
    virtual SceneNodeData serialize() const override;    

private:
    std::vector<std::string> m_flags;
    std::shared_ptr<Entity> m_parent;
    
    bool m_has_flag(const std::string &flag);
};

struct Collider : public Serializable {
    friend class ::Entity;

    Collider() noexcept = default;
    virtual ~Collider() override = default;

    void type_box();
    void type_circle();
    const std::vector<CollisionData> &get_colliding_entities() const;

    std::vector<CollisionData> colliding_entities; // not saved because of frame by frame recalculation
    bool resolve_collisions = true; // accessible from lua
    bool fixed = false; // accessible from lua
    enum Type : uint8_t { AABB=0, Circle } type = Type::AABB;

    virtual void deserialize(SceneNodeData &data) override;
    virtual SceneNodeData serialize() const override;

private:
    std::shared_ptr<Entity> m_parent;
};

} // namespace component