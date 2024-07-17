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

    template<typename... T> 
    decltype(auto) get_view() const { 
        return m_registry->view<T...>(); 
    }

    Camera *get_camera() const {
        assert(m_camera);
        return m_camera;
    }

private:
    std::shared_ptr<entt::registry> m_registry;
    Camera *m_camera = nullptr;

    Entity deserializer_add_entity();
};