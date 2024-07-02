#pragma once

#include "pch.h"
#include <typeindex>

// asserts instead of segfaults in undefined cases, but has a non trivial overhead
#define ECS_SAFETY_CHECKS

namespace ecs {

using Entity = uint32_t;

template<typename T>
class BaseComponent {
public:
    T component;
    Entity parent_ent;
};

class BaseComponentArray {
public:
    virtual ~BaseComponentArray() = default;
};

template<typename T>
class ComponentArray : public BaseComponentArray {
public:
    std::vector<BaseComponent<T>> data;

    operator std::vector<BaseComponent<T>>() {
        return &data;
    }
};

class ComponentStorage {
public:
    Entity add_entity() {
        static Entity curr_index = 0;
        Entity new_entity = ++curr_index;
        m_entities.push_back(new_entity);
        return new_entity;
    }

    template<typename T, typename... Args>
    void add_component(Entity ent, Args&&... args) {
        auto component_array = m_storage[std::type_index(typeid(T))].get();

        if (component_array == nullptr) {
            m_storage[std::type_index(typeid(T))] = std::make_unique<ComponentArray<T>>();
            component_array = m_storage[std::type_index(typeid(T))].get();
        }
#ifdef  ECS_SAFETY_CHECKS
        assert(!has_component<T>(ent) && "Cannot have two components of the same type");
#endif
        m_cast<T>(component_array)->data.push_back(BaseComponent<T>{T{std::forward<Args>(args)...}, ent});
        std::cout << m_cast<T>(component_array)->data.size() << std::endl;
    }

    template<typename T>
    bool has_component(Entity ent) {
        auto component_array = m_storage[std::type_index(typeid(T))].get();
        if (component_array == nullptr) return false;
        
        for (const BaseComponent<T> &it : m_cast<T>(component_array)->data) {
            if (it.parent_ent == ent) {
                return true;
            }
        }
        return false;
    }

    template<typename T>
    std::optional<std::reference_wrapper<T>> get_component(Entity ent) {
        auto component_array = m_storage[std::type_index(typeid(T))].get();
        if (component_array == nullptr) return std::nullopt;

        for (BaseComponent<T> &it : m_cast<T>(component_array)->data) {
            if (it.parent_ent == ent) {
                return std::optional<std::reference_wrapper<T>>(it.component);
            }
        }
        return std::nullopt;
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<BaseComponentArray>> m_storage;
    std::vector<Entity> m_entities;

    template<typename T>
    ComponentArray<T>* m_cast(BaseComponentArray *arr) {
        return static_cast<ComponentArray<T>*>(arr);
    }
};

} // namespace ecs