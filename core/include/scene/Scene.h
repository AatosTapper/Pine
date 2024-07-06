#pragma once

#include "ecs.h"

class Entity;
class Scene {
    friend class Entity;
public:
    Scene() : m_storage(std::make_unique<ecs::ComponentStorage>()) {}
    Scene(Scene &oth) : m_storage(std::move(oth.m_storage)) {}
    Scene operator=(Scene oth) {
        m_storage = std::move(oth.m_storage);
        return *this;
    }
    Scene &operator=(Scene &oth) {
        m_storage = std::move(oth.m_storage);
        return *this;
    }

    Entity add_entity(std::string name = "");

    std::vector<Entity> get_entities();

    template<typename T>
    auto &components() {
        return m_storage->components<T>();
    }
    template<typename... Types, typename Iterator>
    auto get_multiple_components(Iterator it) {
        return m_storage->get_multiple_components<Types...>(it);
    }
    template<typename T> auto begin() { return m_storage->begin<T>(); }
    template<typename T> auto end() { return m_storage->end<T>(); }

private:
    std::unique_ptr<ecs::ComponentStorage> m_storage;
};
