#pragma once

#include "Scene.h"
#include "profiling.h"

class Entity {
    friend class Scene;
public:
    Entity(entt::entity ent, Scene *scene) : m_handle(ent), m_scene(scene) {}
    Entity() = default;
    Entity(Scene *scene) { *this = scene->add_entity(); }

    template<typename T, typename ...Args>
    T& add_component(Args&&... args) {
        if (m_registry().all_of<T>(m_handle)) [[unlikely]] {
            return m_registry().replace<T>(m_handle, std::forward<Args>(args)...);
        } else {
            return m_registry().emplace<T>(m_handle, std::forward<Args>(args)...);
        }
    }
    
    template<typename T>
    T &get_component() {
        return m_registry().get<T>(m_handle);
    }

    /// @brief Get any number of components, only used on C++ side
    template<typename... T>
    auto get() {
        return m_registry().get<T...>(m_handle);
    }

    template<typename... T>
    bool has_component() {
        return m_registry().all_of<T...>(m_handle);
    }

    template<typename T>
    void remove_component() {
        m_registry().remove<T>(m_handle);
    }

    void remove() {
        m_registry().destroy(m_handle);
        m_handle = entt::null;
        m_scene = nullptr;
    }

private:
    entt::entity m_handle{};
    Scene *m_scene = nullptr;

    entt::registry &m_registry() {
        assert(m_scene && "cannot use a removed entity");
        return *m_scene->m_registry;
    }
};