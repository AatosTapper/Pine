#pragma once

#include "Scene.h"
#include "profiling.h"
#include "Components.h"

class Entity {
    friend class Scene;
public:
    Entity(entt::entity ent, Scene *scene) noexcept;
    Entity() noexcept = default;
    Entity(Scene *scene) noexcept;

    bool operator<(const Entity& other) const {
        return m_handle < other.m_handle;
    }
    bool operator==(const Entity& other) const {
        return m_handle == other.m_handle;
    }

    template<typename T, typename ...Args> 
    T& add_component(Args&&... args);

    template<typename T>
    T &get_component();

    template<typename... T> 
    bool has_component();

    template<typename T> 
    void remove_component();

    template<typename T>
    void add_empty_component();

    /// @brief Get any number of components, only used on C++ side
    template<typename... T> auto get();
    template<typename... T> decltype(auto) try_get();

    void remove();

    entt::entity get_entt() const;
    Scene *get_scene() const;

private:
    entt::entity m_handle{};
    Scene *m_scene = nullptr;

    entt::registry &m_registry() {
        assert(m_scene && "cannot use a removed entity");
        return *m_scene->m_registry;
    }

    bool m_invalid() {
        if (!m_registry().valid(m_handle)) {
            std::cout << "attempt to use a null entity\n";
            return true;
        }
        return false;
    }
};


inline Entity::Entity(entt::entity ent, Scene *scene) noexcept : m_handle(ent), m_scene(scene) {}
inline Entity::Entity(Scene *scene) noexcept { *this = scene->add_entity(); }

template<typename T, typename ...Args>
inline T& Entity::add_component(Args&&... args) {
    if (m_invalid()) std::abort();

    if (m_registry().all_of<T>(m_handle)) [[unlikely]] {
        auto &comp = m_registry().replace<T>(m_handle, std::forward<Args>(args)...);
        comp.m_parent = std::make_shared<Entity>(*this);
        return comp;
    } else {
        auto &comp = m_registry().emplace<T>(m_handle, std::forward<Args>(args)...);
        comp.m_parent = std::make_shared<Entity>(*this);
        return comp;
    }
}

template<typename T>
inline T &Entity::get_component() {
    if (m_invalid()) std::abort();
    return m_registry().get<T>(m_handle);
}

template<typename... T>
inline bool Entity::has_component() {
    if (m_invalid()) return false;
    return m_registry().all_of<T...>(m_handle);
}

template<typename T>
inline void Entity::remove_component() {
    if (m_invalid()) return;
    m_registry().remove<T>(m_handle);
}

template<>
inline void Entity::remove_component<component::Transform>() {
    if (m_invalid()) return;
    std::cout << "Error invalid component: can't delete Transform from entity\n";
    std::abort();
}

template<>
inline void Entity::remove_component<component::Tag>() {
    if (m_invalid()) return;
    std::cout << "Error invalid component: can't delete Tag from entity\n";
    std::abort();
}

template<typename T>
inline void Entity::add_empty_component() {
    if (m_registry().all_of<T>(m_handle)) [[unlikely]] {
        m_registry().replace<T>(m_handle);
    } else {
        m_registry().emplace<T>(m_handle);
    }
}

template<typename... T>
inline auto Entity::get() {
    return m_registry().get<T...>(m_handle);
}

template<typename... T>
inline decltype(auto) Entity::try_get() {
    return m_registry().try_get<T...>(m_handle);
}

inline void Entity::remove() {
    if (m_invalid()) return;
    assert(m_scene);
    m_scene->remove_entity(*this);
}

inline entt::entity Entity::get_entt() const {
    return m_handle;
}

inline Scene *Entity::get_scene() const {
    assert(m_scene);
    return m_scene;
}


// Needs to be here to avoid circular references
struct CollisionData {
    Entity ent;
    glm::vec2 normal;
};