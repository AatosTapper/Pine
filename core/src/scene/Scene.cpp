#include "scene/Scene.h"

#include "scene/Components.h"
#include "scene/Entity.h"
#include "ScriptEngine.h"
#include "scene/SceneSerializer.h"

template <typename T>
std::vector<Entity> entt_to_entity(T vec, Scene *scene) {
    std::vector<Entity> output;
    output.reserve(vec.size_hint());
    for (auto &ent : vec) {
        output.emplace_back(ent, scene);
    }
    return output;
}

template <>
std::vector<Entity> entt_to_entity<std::vector<entt::entity>>(std::vector<entt::entity> vec, Scene *scene) {
    std::vector<Entity> output;
    output.reserve(vec.size());
    for (auto &ent : vec) {
        output.emplace_back(ent, scene);
    }
    return output;
}

Scene::Scene(std::string _name) noexcept : 
    name(_name),
    m_registry(std::make_shared<entt::registry>()),
    m_camera(std::make_shared<Camera>()),
    m_remove_queue(std::make_shared<std::vector<Entity>>())
{
    m_camera->back(
        ScriptEngine::get_config_var_double(
            LuaStateDispatcher::instance().get_lua(), "cam_start_z"
        )
    );
    spatial_grid.populate(this);
}

void Scene::update() {
    m_remove_entities();
    spatial_grid.populate(this);
}

Entity Scene::add_entity(std::string name) {
    auto ent = m_registry->create();
    m_registry->emplace<component::Tag>(ent, name.empty() ? "Entity" : name);
    m_registry->emplace<component::Transform>(ent);
    Entity entity{ ent, this };
    spatial_grid.push_entity(entity);
    return entity;
}

std::optional<Entity> Scene::get_entity(const std::string &name) {
    auto entities = m_registry->view<component::Tag>();
    for (auto ent : entities) {
        const auto &tag = this->m_registry->get<component::Tag>(ent);
        if (tag.name == name) {
            return Entity{ ent, this };
        }
    }
    return std::nullopt;
}

void Scene::remove_entity(Entity entity) {
    m_remove_queue->push_back(entity);
}

std::vector<Entity> Scene::get_entities() {
    auto entities = m_registry->view<entt::entity>();
    return entt_to_entity(entities, this);
}

std::vector<Entity> Scene::get_close_entities(Entity ent, float distance) {
    auto &&pos = ent.get<component::Transform>();
    auto output = spatial_lookup(pos.x, pos.y, distance);
    output.erase(std::remove_if(output.begin(), output.end(), [ent](const Entity &oth) {
        return oth.m_handle == ent.m_handle;
    }), output.end());
    return output;
}

Entity Scene::deserializer_add_entity() {
    auto ent = m_registry->create();
    Entity entity{ ent, this };
    return entity;
}

Camera *Scene::get_camera() const {
    return m_camera.get();
}

entt::registry *Scene::get_registry() const { 
    return m_registry.get(); 
}

std::vector<Entity> Scene::spatial_lookup(double x, double y, float radius) {
    return entt_to_entity(spatial_grid.spatial_lookup(x, y, radius, this), this);
}

void Scene::m_remove_entities() {
    for (auto ent : *m_remove_queue) {
        if (ent.m_handle == entt::null) continue;
        m_registry->destroy(ent.m_handle);
        ent.m_handle = entt::null;
    }
    m_remove_queue->clear();
}

void Scene::deserialize(SceneNodeData &data) {
    VAR_FROM_NODE(name, data);

    float cam_x = 0.0f;
    float cam_y = 0.0f;
    float cam_z = 0.0f;
    VAR_FROM_NODE(cam_x, data);
    VAR_FROM_NODE(cam_y, data);
    VAR_FROM_NODE(cam_z, data);
    m_camera->set_position(glm::vec3{ cam_x, cam_y, cam_z });
}

SceneNodeData Scene::serialize() const {
    auto &pos = m_camera->get_position();
    auto cam_x = pos.x;
    auto cam_y = pos.y;
    auto cam_z = pos.z;

    return SceneNodeData {
        .type = SceneNodeType::Scene,
        .variables = {
            VAR_TO_NODE(name),
            VAR_TO_NODE(cam_x),
            VAR_TO_NODE(cam_y),
            VAR_TO_NODE(cam_z)
        }
    };
}

std::unique_ptr<SceneNode> Scene::export_entity(Entity ent) {
    return SceneSerializer::instance().serialize_entity(ent);
}

Entity Scene::import_entity(const std::unique_ptr<SceneNode> &ent) {
    return SceneSerializer::instance().deserialize_entity(this, ent);
}
