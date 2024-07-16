#pragma once

#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"
#include "singleton.h"

class System {
public:
    virtual ~System() {};
    virtual void update(Scene *scene) = 0;
};

class CustomBehaviourSystem : public System {
public:
    IMPL_VIRTUAL_SINGLETON_DISPATCHER(CustomBehaviourSystem, System)
    virtual ~CustomBehaviourSystem() override {}

    virtual void update(Scene *scene) override {
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
};