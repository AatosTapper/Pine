#include "scene/System.h"

#include "scene/Components.h"
#include "scene/Entity.h"
#include "systems/Collision.h"

/// @Lua API
void custom_behavior_system_update(Scene *const scene) {
    auto &lua = LuaStateDispatcher::instance().get_lua();
    
    Entity current_entity;
    lua.set_function("pine_get_script_parent_entity", [&current_entity] { return current_entity; });

    const auto view = scene->get_view<component::CustomBehavior>();
    for (auto ent : view) {
        Entity entity{ ent, scene };
        auto comp = entity.get_component<component::CustomBehavior>();
        if (comp->on_update.empty()) [[unlikely]] continue;

        current_entity = entity;
        ScriptEngine::run_script(lua, app_relative_path(comp->on_update));
    }

    lua.set_function("pine_get_script_parent_entity", [] { 
        std::cerr << "Script Error: Cannot call pine_get_script_parent_entity outside a valid on_update context\n";
        std::abort();
    });
}

void update_transform_component_last_states(const Scene *const scene) {
    const auto group = scene->get_registry()->group<component::Transform>();
    group.each([](component::Transform &comp) {
        comp.save_last_position();
    });
}

void interpolate_transform_components(const Scene *const scene, float alpha) {
    const auto group = scene->get_registry()->group<component::Transform>();
    group.each([alpha](component::Transform &t1) {
        t1.interpolate(alpha);
    });
}

static void cell_collision(const std::vector<entt::entity> &entities, Scene *const scene, entt::registry *const registry, const uint8_t iter) {
    const size_t num = (size_t)((int64_t)entities.size() - 1);
    for (size_t i = 0; i < num; i++) {
        if (!registry->all_of<component::Collider, component::Transform>(entities[i])) {
            continue;
        }
        const auto ent_i = entities[i];
        for (size_t j = i + 1; j < entities.size(); j++) {
            if (!registry->all_of<component::Collider, component::Transform>(entities[j])) {
                continue;
            }
            if (auto result = collide(ent_i, entities[j], scene)) {
                if (iter > 0) {
                    continue;
                }
                glm::vec2 normal = *result;
                registry->get<component::Collider>(ent_i).colliding_entities.emplace_back(
                    CollisionData{ 
                        .ent = Entity{ entities[j], scene }, 
                        .normal = normal * (-1.0f)
                    }
                );
                registry->get<component::Collider>(entities[j]).colliding_entities.emplace_back(
                    CollisionData{ 
                        .ent = Entity{ ent_i, scene }, 
                        .normal = normal
                    }
                );
            }
        }
    }
}

void collision_system_update(Scene *scene) {
    static bool is_first_round = true;
    if (is_first_round) {
        is_first_round = false;
        return;
    }
    const auto registry = scene->get_registry();
    registry->group<component::Collider>().each([](component::Collider &collider) {
        collider.colliding_entities.clear();
    });
    static constexpr uint8_t iterations = 2u;
    for (uint8_t iter = 0; iter < iterations; iter++) {
        scene->spatial_grid.for_all_cells([scene, registry, iter](const std::vector<entt::entity> &entities) {
            cell_collision(entities, scene, registry, iter);
        });
    }
}