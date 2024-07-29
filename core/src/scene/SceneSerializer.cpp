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
    root->data = scene->serialize();
    
    auto *registry = scene->m_registry.get();
    for (auto ent : registry->view<entt::entity>()) {
        if (registry->all_of<EntityDoNotSerialize>(ent)) continue;

        auto [tag, trans, cb, script, table, sprite, flags, coll] = registry->try_get<
            component::Tag,
            component::Transform,
            component::CustomBehavior,
            component::Script,
            component::Table,
            component::Sprite, 
            component::StateFlags,
            component::Collider>(ent);

        auto entity = std::make_unique<Node>(Node { .data = { .type = NodeType::Entity }, .parent = root.get() });
        serialize_comp(entity, tag);
        serialize_comp(entity, trans);
        serialize_comp(entity, cb);
        serialize_comp(entity, script);
        serialize_comp(entity, table);
        serialize_comp(entity, sprite);
        serialize_comp(entity, flags);
        serialize_comp(entity, coll);
        root->children.push_back(std::move(entity));
    }
    write_scene(root, path.c_str());
}

#define FIRST_COMP_FROM_TYPE(TYPE) \
    if (comp_type == #TYPE) { component = &entity.add_component<component::TYPE>(); }
#define COMP_FROM_TYPE(TYPE) else FIRST_COMP_FROM_TYPE(TYPE)

static void deserialize_components(const std::unique_ptr<Node> &ent, Entity entity) {
    for (const auto &comp : ent->children) {
        assert(comp->data.type == NodeType::Component && "Deserialize error: scene_graph incorrect structure at component level");
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

std::unique_ptr<Scene> SceneSerializer::deserialize(std::string path) {
    PINE_CORE_PROFILE("Full scene deserialize");

    std::unique_ptr<Node> scene_graph = read_scene(path.c_str());
    assert(scene_graph->data.type == NodeType::Scene && "Deserialize error: scene_graph incorrect structure at scene level");
    
    std::unique_ptr<Scene> scene = std::make_unique<Scene>("");
    scene->deserialize(scene_graph->data);

    for (const auto &ent : scene_graph->children) {
        assert(ent->data.type == NodeType::Entity && "Deserialize error: scene_graph incorrect structure at entity level");
        Entity entity = scene->deserializer_add_entity();
        deserialize_components(ent, entity);
    }

    return scene;
}