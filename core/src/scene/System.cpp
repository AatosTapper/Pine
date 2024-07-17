#include "scene/System.h"

#include "scene/Components.h"
#include "scene/Entity.h"

void custom_behaviour_system_update(Scene *scene) {
    auto &lua = LuaStateDispatcher::instance().get_lua();
    
    Entity current_entity;
    lua.set_function("pine_get_script_parent_entity", [&current_entity] { return current_entity; });

    for (auto ent : scene->get_view<component::CustomBehaviour>()) {
        Entity entity{ent, scene};
        auto &customBehaviour = entity.get_component<component::CustomBehaviour>();
        if (customBehaviour.on_update.empty()) [[unlikely]] continue;

        current_entity = entity;
        ScriptEngine::run_script(lua, app_relative_path(customBehaviour.on_update));
    }
}

void update_transform_component_last_states(Scene *scene) {
    auto group = scene->get_registry()->group<component::Transform>();
    group.each([](component::Transform &comp) {
        comp.save_last_position();
    });
}

void interpolate_transform_components(Scene *scene, float alpha) {
    auto group = scene->get_registry()->group<component::Transform>();
    group.each([alpha](component::Transform &comp) {
        comp.interpolate(alpha);
    });
}