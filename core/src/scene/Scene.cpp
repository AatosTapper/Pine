#include "scene/Scene.h"

#include "scene/Components.h"
#include "scene/Entity.h"

Scene::Scene() {
    
}

Scene::~Scene() {

}

Entity Scene::add_entity(std::string name) {
    auto ent = m_registry.create();
    m_registry.emplace<component::Tag>(ent, name.empty() ? "Entity" : name);
    m_registry.emplace<component::Transform>(ent);

    return Entity{ ent, this };
}

std::vector<Entity> Scene::get_entities() {
    auto entities = m_registry.view<entt::entity>();
    std::vector<Entity> output;
    output.reserve(entities.size_hint());
    for (auto &ent : entities) {
        output.push_back(Entity{ ent, this });
    }
    return output;
}