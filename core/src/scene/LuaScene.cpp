#include "scene/LuaScene.h"

#include "pch.h"
#include "scene/Scene.h"
#include "scene/Entity.h"
#include "scene/Components.h"
#include "scene/SceneManager.h"

template<typename T>
sol::optional<T&> opt_to_sol(std::optional<std::reference_wrapper<T>> opt) {
    if (opt) {
        return opt->get();
    } else {
        return sol::nullopt;
    }
}

#define _REG(FUNC, TYPE) \
    entity_type[#FUNC"_component_"#TYPE] = [](Entity &self) { \
        return self.FUNC##_component<component::TYPE>(); \
    };
#define _REG_OPT(FUNC, TYPE) \
    entity_type[#FUNC"_component_"#TYPE] = [](Entity &self) { \
        return opt_to_sol(self.FUNC##_component<component::TYPE>()); \
    };
#define FUNC_REGISTER(TYPE) \
    _REG(add, TYPE) \
    _REG_OPT(get, TYPE) \
    _REG(remove, TYPE) \
    _REG(has, TYPE)
#define COMP_REGISTER(TYPE) \
    sol::usertype<component::TYPE> TYPE##_type = lua.new_usertype<component::TYPE>("pine_comp_"#TYPE, \
        sol::constructors<component::TYPE()>());

#define COMP_VAR_REGISTER(TYPE, NAME) \
    TYPE##_type[#NAME] = &component::TYPE::NAME; \

void set_lua_entity(sol::state &lua) {
    sol::usertype<Entity> entity_type = lua.new_usertype<Entity>("pine_Entity",
        sol::constructors<Entity(), Entity(Scene *scene)>());

    entity_type["remove"] = &Entity::remove;

    FUNC_REGISTER(Tag)
    FUNC_REGISTER(Transform)
    FUNC_REGISTER(Script)
    FUNC_REGISTER(VarInt)
}

void set_lua_components(sol::state &lua) {
    COMP_REGISTER(Tag)
    COMP_VAR_REGISTER(Tag, name)

    COMP_REGISTER(Transform)
    COMP_VAR_REGISTER(Transform, x)
    COMP_VAR_REGISTER(Transform, y)
    COMP_VAR_REGISTER(Transform, sx)
    COMP_VAR_REGISTER(Transform, sy)

    COMP_REGISTER(Script)
    COMP_VAR_REGISTER(Script, src)

    COMP_REGISTER(VarInt)
    COMP_VAR_REGISTER(VarInt, var)
}

void set_lua_scene(sol::state &lua, SceneManager &manager) {
    sol::usertype<Scene> scene_type = lua.new_usertype<Scene>("pine_Scene",
        sol::constructors<Scene()>());

    scene_type["add_entity"] = &Scene::add_entity;

    lua.set_function("pine_set_scene", [&](Scene scene) {
        manager.push(std::make_unique<Scene>(scene));
    });
}