#include "Scene/SceneSerializer.h"

#include "Serialize.h"
#include "SceneParser.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

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
    std::unique_ptr<Node> root = std::make_unique<Node>();
    root->data.type = NodeType::Scene;
    
    auto *registry = scene->m_registry.get();
    for (auto ent : registry->view<entt::entity>()) {
        auto [tag, trans, cb, script, sprite] = registry->try_get<component::Tag, component::Transform, component::CustomBehaviour, component::Script, component::Sprite>(ent);

        auto entity = std::make_unique<Node>(Node { .data = { .type = NodeType::Entity }, .parent = root.get() });
        serialize_comp(entity, tag);
        serialize_comp(entity, trans);
        serialize_comp(entity, cb);
        serialize_comp(entity, script);
        serialize_comp(entity, sprite);
        root->children.push_back(std::move(entity));
    }

    root->print_tree();
    write_scene(root, path.c_str());
}

std::unique_ptr<Scene> SceneSerializer::deserialize(std::string path) {

}
