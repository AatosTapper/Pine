#pragma once

#include "entt/entt.hpp"
#include "rendering/Camera.h"

class Entity;
class Renderer;
class SceneManager;
class Scene {
    friend class Entity;
    friend class SceneManager;
public:
    Scene() noexcept;
    Scene(const Scene &oth) noexcept;
    
    Entity add_entity(std::string name = "");

    std::vector<Entity> get_entities();

    template<typename... T>
    auto view() { return m_registry->view<T...>(); }

    Camera *get_camera() const { assert(m_camera); return m_camera; }

private:
    std::shared_ptr<entt::registry> m_registry;
    Camera *m_camera = nullptr;
};
