#pragma once

#include "pch.h"
#include <typeindex>
#include <tuple>

// asserts instead of segfaults in undefined cases, but has a non trivial overhead
#define ECS_SAFETY_CHECKS

namespace ecs {

using entity_id = uint32_t;

template<typename T>
class BaseComponent {
    friend class ComponentStorage;
public:
    BaseComponent(T _component, entity_id ent) : component(_component), m_parent_ent(ent) {}
    T component;
    
    T& operator*() {
        return component;
    }
    T* operator->() {
        return &component;
    }
    entity_id get_entity() const {
        return m_parent_ent;
    }

private:
    entity_id m_parent_ent;
};

class BaseComponentArray {
public:
    virtual ~BaseComponentArray() = default;

    std::function<void(entity_id)> removal_function;
};

template<typename T>
class ComponentArray : public BaseComponentArray {
public:
    ComponentArray(std::function<void(entity_id)> _removal_func) {
        removal_function = _removal_func;
    }
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
    entity_id add_entity() {
        static entity_id curr_index = 0;
        entity_id new_entity = ++curr_index;
        return new_entity;
    }

    template<typename T, typename... Args>
    void add_component(entity_id ent, Args&&... args) {
        auto component_array = m_get_or_create_component_array<T>();
#ifdef  ECS_SAFETY_CHECKS
        assert(!has_component<T>(ent) && "Cannot have two components of the same type");
#endif
        component_array->data.push_back(BaseComponent<T>{T{std::forward<Args>(args)...}, ent});
        m_entities[ent].push_back(std::type_index(typeid(T)));
    }

    template<typename T>
    bool has_component(entity_id ent) {
        for (auto it : m_entities[ent]) {
            if (it == std::type_index(typeid(T))) return true;
        }
        return false;
    }

    bool _rt_has_component(entity_id ent, std::type_index type) {
        for (auto it : m_entities[ent]) {
            if (it == type) return true;
        }
        return false;
    }

    template<typename T>
    std::optional<std::reference_wrapper<T>> get_component(entity_id ent) {
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
    void remove_component(entity_id ent) {
        auto component_array = m_cast<T>(m_storage.at(std::type_index(typeid(T))).get());
        if (component_array == nullptr) return;
#ifdef  ECS_SAFETY_CHECKS
        assert(has_component<T>(ent) && "Cannot remove a component that doesn't exist");
#endif
        component_array->data.erase(std::remove_if(component_array->data.begin(), component_array->data.end(),
            [ent](const BaseComponent<T> &comp) {
                return comp.m_parent_ent == ent;
            }
        ), component_array->data.end());
        m_storage[std::type_index(typeid(T))] = std::make_unique<ComponentArray<T>>(*component_array);

        m_entities[ent].erase(std::remove_if(m_entities[ent].begin(), m_entities[ent].end(),
            [](const auto pos) {
                return std::type_index(typeid(T)) == pos;
            }
        ), m_entities[ent].end());
    }

    void remove_entity(entity_id ent) {
        std::vector<std::type_index> component_vec = m_entities[ent];
        for (auto comp : component_vec) {
            std::cout << "Removed component for entity " << ent << std::endl;
            m_storage.at(comp)->removal_function(ent);
        }
        m_entities.erase(ent);
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

    template<typename T>
    ComponentArray<T> &components() {
        auto component_array = m_get_or_create_component_array<T>();
        return *m_cast<T>(component_array);
    }

    template<typename... Types, typename Iterator>
    std::tuple<std::optional<std::reference_wrapper<Types>>...> get_multiple_components(Iterator it) {
        return std::make_tuple(get_component<Types>(it->m_parent_ent)...);
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<BaseComponentArray>> m_storage;
    std::unordered_map<entity_id, std::vector<std::type_index>> m_entities;

    template<typename T>
    auto m_cast(BaseComponentArray *arr) const {
        return static_cast<ComponentArray<T>*>(arr);
    }

    template<typename T>
    auto m_get_or_create_component_array() {
        auto component_array = m_storage[std::type_index(typeid(T))].get();
        if (component_array == nullptr) {
            m_storage[std::type_index(typeid(T))] = std::make_unique<ComponentArray<T>>([this](entity_id ent) {
                this->remove_component<T>(ent);
            });
            component_array = m_storage[std::type_index(typeid(T))].get();
        }
        return m_cast<T>(component_array);
    }
};

} // namespace ecs