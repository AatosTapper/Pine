#pragma once

#include "entt/entt.hpp"

class Entity;
class Renderer;
class Scene {
    friend class Entity;
    friend class Renderer;
public:
    Scene();
    Scene(Scene &oth) {
        m_registry = std::move(oth.m_registry);
    }
    ~Scene();

    Entity add_entity(std::string name = "");
    std::vector<Entity> get_entities();

    template<typename... T>
    auto view() { return m_registry.view<T...>(); }

private:
    entt::registry m_registry;
};
