#include "scene/SceneSerializer.h"

#include "Serialize.h"
#include "scene/SceneParser.h"
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


static void serialize_comp(std::unique_ptr<SceneNode> &parent, Serializable *component) {
    if (!component) return;
    parent->children.push_back(std::make_unique<SceneNode>(
        SceneNode {
            .data = component->serialize(),
            .parent = parent.get()
        }
    ));
}

#define FIRST_COMP_FROM_TYPE(TYPE) \
    if (comp_type == #TYPE) { component = &entity.add_component<component::TYPE>(); }
#define COMP_FROM_TYPE(TYPE) else FIRST_COMP_FROM_TYPE(TYPE)

static void deserialize_components(const std::unique_ptr<SceneNode> &ent, Entity entity) {
    for (const auto &comp : ent->children) {
        assert(comp->data.type == SceneNodeType::Component && "Deserialize error: scene_graph incorrect structure at component level");
        std::string comp_type = comp->data.variables.at(0).value;
        Serializable *component = nullptr;

        FIRST_COMP_FROM_TYPE(Tag)
        COMP_FROM_TYPE(Transform)
        COMP_FROM_TYPE(CustomBehavior)
        COMP_FROM_TYPE(Script)
        COMP_FROM_TYPE(Table)
        COMP_FROM_TYPE(Sprite)
        COMP_FROM_TYPE(StateFlags)
        COMP_FROM_TYPE(Collider)
        else {
            std::cerr << "Deserialize error: component type \"" << comp_type << "\" not found\n";
            std::abort();
        }
        component->deserialize(comp->data);
    }
}

std::unique_ptr<SceneNode> SceneSerializer::serialize_entity(Entity entity) {
    auto [tag, trans, cb, script, table, sprite, flags, coll] = entity.try_get<
        component::Tag,
        component::Transform,
        component::CustomBehavior,
        component::Script,
        component::Table,
        component::Sprite, 
        component::StateFlags,
        component::Collider>();

    auto node = std::make_unique<SceneNode>(SceneNode { .data = { .type = SceneNodeType::Entity }, .parent = nullptr });
    serialize_comp(node, tag);
    serialize_comp(node, trans);
    serialize_comp(node, cb);
    serialize_comp(node, script);
    serialize_comp(node, table);
    serialize_comp(node, sprite);
    serialize_comp(node, flags);
    serialize_comp(node, coll);

    return node;
}

Entity SceneSerializer::deserialize_entity(Scene *scene, const std::unique_ptr<SceneNode> &node) {
    Entity entity = scene->deserializer_add_entity();
    deserialize_components(node, entity);
    return entity;
}


void SceneSerializer::serialize(Scene *scene, std::string path) {
    PINE_CORE_PROFILE("Full scene serialize");

    std::unique_ptr<SceneNode> root = std::make_unique<SceneNode>();
    root->data = scene->serialize();
    
    auto *registry = scene->m_registry.get();
    for (auto ent : registry->view<entt::entity>()) {
        if (registry->all_of<EntityDoNotSerialize>(ent)) continue;

        auto entity = serialize_entity(Entity{ ent, scene });
        entity->parent = root.get();
        root->children.push_back(std::move(entity));
    }
    write_scene(root, path.c_str());
}

std::unique_ptr<Scene> SceneSerializer::deserialize(std::string path) {
    PINE_CORE_PROFILE("Full scene deserialize");

    std::unique_ptr<SceneNode> scene_graph = read_scene(path.c_str());
    assert(scene_graph->data.type == SceneNodeType::Scene && "Deserialize error: scene_graph incorrect structure at scene level");
    
    std::unique_ptr<Scene> scene = std::make_unique<Scene>("");
    scene->deserialize(scene_graph->data);

    for (const auto &ent_node : scene_graph->children) {
        assert(ent_node->data.type == SceneNodeType::Entity && "Deserialize error: scene_graph incorrect structure at entity level");
        deserialize_entity(scene.get(), ent_node);
    }

    return scene;
}