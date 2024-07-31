#pragma once

#include "entt/entt.hpp"
#include "rendering/Camera.h"
#include "Serialize.h"
#include "SpatialGrid.h"

#include <optional>

class Entity;
class SceneManager;
class Scene : public Serializable {
    friend class Entity;
    friend class SceneManager;
    friend class SceneSerializer;
public:
    Scene(std::string _name) noexcept;
    virtual ~Scene() override = default;

    std::string name;

    void update();
    
    Entity add_entity(std::string name = "");
    std::optional<Entity> get_entity(const std::string &name);
    void remove_entity(Entity entity);
    std::vector<Entity> get_entities();
    std::vector<Entity> get_close_entities(Entity ent, float distance);
    Camera *get_camera() const;
    template<typename... T> decltype(auto) get_view() const;
    entt::registry *get_registry() const;

    std::vector<Entity> spatial_lookup(double x, double y, float radius);

    virtual void deserialize(SceneNodeData &data) override;
    virtual SceneNodeData serialize() const override;

    std::unique_ptr<SceneNode> export_entity(Entity ent);
    Entity import_entity(const std::unique_ptr<SceneNode> &ent);

    SpatialGrid spatial_grid;

private:
    std::shared_ptr<entt::registry> m_registry;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<std::vector<Entity>> m_remove_queue;

    Entity deserializer_add_entity();

    void m_remove_entities();
};

template<typename... T> 
decltype(auto) Scene::get_view() const { 
    return m_registry->view<T...>(); 
}