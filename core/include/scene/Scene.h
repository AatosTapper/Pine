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
    Scene() : m_registry(std::make_shared<entt::registry>()) {}
    Scene(const Scene &oth) { m_registry = oth.m_registry; }
    
    Entity add_entity(std::string name = "");

    std::vector<Entity> get_entities();

    template<typename... T>
    auto view() { return m_registry->view<T...>(); }

    Camera *get_camera() const { assert(m_camera); return m_camera; }

private:
    std::shared_ptr<entt::registry> m_registry;
    Camera *m_camera = nullptr;
};
