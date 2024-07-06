#include "scene/Scene.h"

#include "scene/Components.h"
#include "scene/Entity.h"

Entity Scene::add_entity(std::string name) {
    auto ent = m_storage->add_entity();
    m_storage->add_component<component::Tag>(ent, name.empty() ? "Entity" : name);
    m_storage->add_component<component::Transform>(ent);

    return Entity{ ent, this };
}

std::vector<Entity> Scene::get_entities() {
    const auto &entities = m_storage->entities();
    std::vector<Entity> output;
    output.reserve(entities.size());

    for (auto it = entities.begin(); it != entities.end(); it++) {
        output.push_back(Entity{ it->first, this });
    }
    return output;
}