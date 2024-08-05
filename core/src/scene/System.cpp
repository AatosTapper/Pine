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
        std::cerr << "Script Error: Cannot call pine_get_script_parent_entity outside the correct on_update context\n";
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
    group.each([alpha](component::Transform &comp) {
        comp.interpolate(alpha);
    });
}

static bool is_first_round = true;
void collision_system_update(Scene *scene) {
    if (is_first_round) [[unlikely]] {
        is_first_round = false;
        return;
    }
    scene->spatial_grid.for_all_cells([scene](const std::vector<entt::entity> &entities) {
        for (int64_t i = 0; i < (int64_t)entities.size() - 1; i++) {    
            if (!scene->get_registry()->all_of<component::Collider, component::Transform>(entities[i])) continue;
            const auto ent_i = entities[i];
            for (size_t j = i + 1; j < entities.size(); j++) {
                if (!scene->get_registry()->all_of<component::Collider, component::Transform>(entities[j])) continue;

                collide(ent_i, entities[j], scene);
            }
        }
    });
}