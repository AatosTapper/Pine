#pragma once

#include "ecs.h"

class Entity;
class Scene {
    friend class Entity;
public:
    Entity add_entity(std::string name = "");

    template<typename T>
    auto &components() {
        return m_storage.components<T>();
    }
    template<typename... Types, typename Iterator>
    auto get_multiple_components(Iterator it) {
        return m_storage.get_multiple_components<Types...>(it);
    }
    template<typename T> auto begin() { return m_storage.begin<T>(); }
    template<typename T> auto end() { return m_storage.end<T>(); }

private:
    ecs::ComponentStorage m_storage;
};
