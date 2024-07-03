#pragma once

#include "ecs.h"
#include "Scene.h"

class Entity {
    friend class Scene;
public:
    Entity(ecs::entity_id ent, Scene *scene) : m_handle(ent), m_scene(scene) {}

    template<typename T, typename ...Args>
    void add_component(Args&&... args) {
        m_storage().add_component<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T>
    std::optional<std::reference_wrapper<T>> get_component() {
        return m_storage().get_component<T>(m_handle);
    }

    template<typename T>
    bool has_component() {
        return m_storage().has_component<T>(m_handle);
    }

    template<typename T>
    void remove_component() {
        m_storage().remove_component<T>(m_handle);
    }

    void remove() {
        m_storage().remove_entity(m_handle);
        m_handle = 0;
        m_scene = nullptr;
    }

private:
    ecs::entity_id m_handle = 0;
    Scene *m_scene = nullptr;

    ecs::ComponentStorage &m_storage() {
        assert(m_scene && "cannot use a removed entity");
        return m_scene->m_storage;
    }
};