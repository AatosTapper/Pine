#pragma once

#include "pch.h"
#include <typeindex>
#include <tuple>

// asserts instead of segfaults in undefined cases, but has a non trivial overhead
#define ECS_SAFETY_CHECKS

namespace ecs {

using Entity = uint32_t;

template<typename T>
class BaseComponent {
    friend class ComponentStorage;
public:
    BaseComponent(T _component, Entity ent) : component(_component), m_parent_ent(ent) {}
    T component;
    
    T& operator*() {
        return component;
    }
    T* operator->() {
        return &component;
    }

    Entity get_entity() const {
        return m_parent_ent;
    }

private:
    Entity m_parent_ent;
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
        return data;
    }

    typename std::vector<BaseComponent<T>>::iterator begin() {
        return data.begin();
    }

    typename std::vector<BaseComponent<T>>::iterator end() {
        return data.end();
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
        auto component_array = m_get_or_create_component_array<T>();

#ifdef  ECS_SAFETY_CHECKS
        assert(!has_component<T>(ent) && "Cannot have two components of the same type");
#endif
        component_array->data.push_back(BaseComponent<T>{T{std::forward<Args>(args)...}, ent});
        std::cout << component_array->data.size() << std::endl;
    }

    template<typename T>
    bool has_component(Entity ent) const {
        auto component_array = m_storage.at(std::type_index(typeid(T))).get();
        if (component_array == nullptr) return false;
        
        for (const BaseComponent<T> &it : m_cast<T>(component_array)->data) {
            if (it.m_parent_ent == ent) {
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
            if (it.m_parent_ent == ent) {
                return std::optional<std::reference_wrapper<T>>(it.component);
            }
        }
        return std::nullopt;
    }

    template<typename T>
    std::vector<BaseComponent<T>>::iterator begin() {
        auto component_array = m_get_or_create_component_array<T>();
        return component_array->data.begin();
    }

    template<typename T>
    std::vector<BaseComponent<T>>::iterator end() {
        auto component_array = m_get_or_create_component_array<T>();
        return component_array->data.end();
    }

    template<typename... Types, typename Iterator>
    std::tuple<std::optional<std::reference_wrapper<Types>>...> get_multiple_components(Iterator it) {
        return std::make_tuple(get_component<Types>(it->get_entity())...);
    }

    template<typename T>
    ComponentArray<T> &components() {
        auto component_array = m_get_or_create_component_array<T>();
        return *m_cast<T>(component_array);
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<BaseComponentArray>> m_storage;
    std::vector<Entity> m_entities;

    template<typename T>
    auto m_cast(BaseComponentArray *arr) const {
        return static_cast<ComponentArray<T>*>(arr);
    }

    template<typename T>
    auto m_get_or_create_component_array() {
        auto component_array = m_storage[std::type_index(typeid(T))].get();
        if (component_array == nullptr) {
            m_storage[std::type_index(typeid(T))] = std::make_unique<ComponentArray<T>>();
            component_array = m_storage[std::type_index(typeid(T))].get();
        }
        return m_cast<T>(component_array);
    }
};

} // namespace ecs