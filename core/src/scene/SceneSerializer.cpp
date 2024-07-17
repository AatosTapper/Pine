#include "scene/SceneSerializer.h"

#include "Serialize.h"
#include "SceneParser.h"
#include "scene/Components.h"
#include "scene/Entity.h"
#include "profiling.h"

#include <filesystem>

void SceneSerializer::save_current(SceneManager &manager) {
    Scene *scene = manager.get_scene();
    assert(scene && "Scene save error: no scene selected");
    SceneSerializer::instance().serialize(scene, app_relative_path("scenes/" + scene->name + ".xml"));
}

Scene *SceneSerializer::load_from_name(SceneManager &manager, const std::string &name) {
    const std::string path = app_relative_path("scenes/" + name + ".xml");
    if (!std::filesystem::exists(path)) { 
        return nullptr;
    }
    auto scene = SceneSerializer::instance().deserialize(path);
    Scene *pointer = scene.get();
    manager.push(std::move(scene));
    manager.try_update();
    return pointer;
}


static void serialize_comp(std::unique_ptr<Node> &parent, Serializable *component) {
    if (!component) return;
    parent->children.push_back(std::make_unique<Node>(
        Node {
            .data = component->serialize(),
            .parent = parent.get()
        }
    ));
}

void SceneSerializer::serialize(Scene *scene, std::string path) {
    PINE_CORE_PROFILE("Full scene serialize");
    std::unique_ptr<Node> root = std::make_unique<Node>();
    root->data.type = NodeType::Scene;
    root->data.variables.push_back(NodeVariable{ .name="name", .value=scene->name });
    
    auto *registry = scene->m_registry.get();
    for (auto ent : registry->view<entt::entity>()) {
        if (registry->all_of<EntityDoNotSerialize>(ent)) continue;

        auto [tag, trans, cb, script, table, sprite, flags] = registry->try_get<
            component::Tag, 
            component::Transform, 
            component::CustomBehaviour, 
            component::Script, 
            component::Table,
            component::Sprite, 
            component::StateFlags>(ent);

        auto entity = std::make_unique<Node>(Node { .data = { .type = NodeType::Entity }, .parent = root.get() });
        serialize_comp(entity, tag);
        serialize_comp(entity, trans);
        serialize_comp(entity, cb);
        serialize_comp(entity, script);
        serialize_comp(entity, table);
        serialize_comp(entity, sprite);
        serialize_comp(entity, flags);
        root->children.push_back(std::move(entity));
    }
    write_scene(root, path.c_str());
}

static void deserialize_components(const std::unique_ptr<Node> &ent, Entity entity) {
    for (const auto &comp : ent->children) {
        assert(comp->data.type == NodeType::Component && "Deserialize error: scene_graph incorrect structure at component level");
    
        std::string comp_type = comp->data.variables.at(0).value;
        Serializable *component;
        if (comp_type == "Tag") {
            component = &entity.add_component<component::Tag>();
        } else if (comp_type == "Transform") {
            component = &entity.add_component<component::Transform>();
        } else if (comp_type == "CustomBehaviour") {
            component = &entity.add_component<component::CustomBehaviour>();
        } else if (comp_type == "Script") {
            component = &entity.add_component<component::Script>();
        } else if (comp_type == "Table") {
            component = &entity.add_component<component::Table>();
        } else if (comp_type == "Sprite") {
            component = &entity.add_component<component::Sprite>();
        } else if (comp_type == "StateFlags") {
            component = &entity.add_component<component::StateFlags>();
        } else {
            assert(false && "Deserialize error: component type not found");
        }
        component->deserialize(comp->data);
    }
}

std::unique_ptr<Scene> SceneSerializer::deserialize(std::string path) {
    PINE_CORE_PROFILE("Full scene deserialize");
    std::unique_ptr<Node> scene_graph = read_scene(path.c_str());
    assert(scene_graph->data.type == NodeType::Scene && "Deserialize error: scene_graph incorrect structure at scene level");
    
    std::unique_ptr<Scene> scene = std::make_unique<Scene>(scene_graph->data.variables.at(0).value);

    for (const auto &ent : scene_graph->children) {
        assert(ent->data.type == NodeType::Entity && "Deserialize error: scene_graph incorrect structure at entity level");
        Entity entity = scene->deserializer_add_entity();
        deserialize_components(ent, entity);
    }

    return scene;
}