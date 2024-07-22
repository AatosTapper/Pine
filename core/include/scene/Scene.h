#pragma once

#include "entt/entt.hpp"
#include "rendering/Camera.h"

class Entity;
class SceneManager;
class Scene {
    friend class Entity;
    friend class SceneManager;
    friend class SceneSerializer;
public:
    Scene(std::string _name) noexcept;
    Scene(const Scene &oth) noexcept;

    std::string name;
    
    Entity add_entity(std::string name = "");
    std::vector<Entity> get_entities();
    /// @todo OPTIMIZE (should not take 35 micro secs :DDD)
    std::vector<Entity> get_close_entities(Entity ent, float distance);

    template<typename... T> decltype(auto) get_view() const;
    Camera *get_camera() const;
    entt::registry *get_registry() const;

private:
    std::shared_ptr<entt::registry> m_registry;
    std::shared_ptr<Camera> m_camera;

    Entity deserializer_add_entity();
};

template<typename... T> 
decltype(auto) Scene::get_view() const { 
    return m_registry->view<T...>(); 
}