#include "scene/Scene.h"

#include "scene/Components.h"
#include "scene/Entity.h"

Scene::Scene(std::string _name) noexcept : 
    name(_name),
    m_registry(std::make_shared<entt::registry>())
{
}

Scene::Scene(const Scene &oth) noexcept { 
    m_registry = oth.m_registry; 
    name = oth.name;
}

Entity Scene::add_entity(std::string name) {
    auto ent = m_registry->create();
    m_registry->emplace<component::Tag>(ent, name.empty() ? "Entity" : name);
    m_registry->emplace<component::Transform>(ent);

    return Entity{ ent, this };
}

std::vector<Entity> Scene::get_entities() {
    auto entities = m_registry->view<entt::entity>();
    std::vector<Entity> output;
    output.reserve(entities.size_hint());
    for (auto &ent : entities) {
        output.push_back(Entity{ ent, this });
    }
    return output;
}

std::vector<Entity> Scene::get_close_entities(Entity ent, float distance) {
    PINE_CORE_PROFILE("Get closeby entities");
    auto &pos = ent.get_component<component::Transform>();
    std::vector<Entity> output;
    for (auto id : m_registry->view<component::Transform>()) {
        if (id == ent.m_handle) [[unlikely]] continue;

        auto &oth = m_registry->get<component::Transform>(id);
        float dx = oth.x - pos.x;
        float dy = oth.y - pos.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if ((dist - distance) < 0.0f) [[unlikely]] {
            output.push_back(Entity{ id, this });
        }
    }
    return output;
}

Entity Scene::deserializer_add_entity() {
    auto ent = m_registry->create();
    return Entity{ ent, this };
}

Camera *Scene::get_camera() const {
    return m_camera.get();
}

entt::registry *Scene::get_registry() const { 
    return m_registry.get(); 
}