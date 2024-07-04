#include "scene/Scene.h"

#include "scene/Components.h"
#include "scene/Entity.h"

Entity Scene::add_entity(std::string name) {
    auto ent = m_storage->add_entity();
    m_storage->add_component<component::Tag>(ent, name.empty() ? "Entity" : name);
    m_storage->add_component<component::Transform>(ent);
    
    return Entity{ent, this};
}