#include "scene/LuaScene.h"

#include "pch.h"
#include "scene/Scene.h"
#include "scene/Entity.h"
#include "scene/Components.h"
#include "scene/SceneManager.h"
#include "scene/SceneSerializer.h"

#define _REG_ADD(FUNC, TYPE, ARG) \
    entity_type[#FUNC"_component_"#TYPE] = [&](Entity &self, sol::variadic_args va) { \
        if (va.size() == 0) { \
            return sol::make_object(lua, &self.FUNC##_component<component::TYPE>()); \
        } \
        return sol::make_object(lua, &self.FUNC##_component<component::TYPE>(va.get<ARG>())); \
    };
#define _REG(FUNC, TYPE) \
    entity_type[#FUNC"_component_"#TYPE] = [&](Entity &self) { \
        return self.FUNC##_component<component::TYPE>(); \
    };
#define _REG_OBJ(FUNC, TYPE) \
    entity_type[#FUNC"_component_"#TYPE] = [&](Entity &self) { \
        return sol::make_object(lua, &self.FUNC##_component<component::TYPE>()); \
    };

#define FUNC_REGISTER(TYPE, CONSTRUCT_ARG) \
    _REG_ADD(add, TYPE, CONSTRUCT_ARG) \
    _REG_OBJ(get, TYPE) \
    _REG(remove, TYPE) \
    _REG(has, TYPE)
#define FUNC_REGISTER_NA(TYPE) \
    _REG(add, TYPE) \
    _REG_OBJ(get, TYPE) \
    _REG(remove, TYPE) \
    _REG(has, TYPE)

#define COMP_REGISTER_NA(TYPE) \
    sol::usertype<component::TYPE> TYPE##_type = lua.new_usertype<component::TYPE>("pine_comp_"#TYPE, \
        sol::constructors<component::TYPE()>());

#define COMP_REGISTER(TYPE, ARGS) \
    sol::usertype<component::TYPE> TYPE##_type = lua.new_usertype<component::TYPE>("pine_comp_"#TYPE, \
        sol::constructors<component::TYPE(), component::TYPE(ARGS)>());

#define COMP_MEM_REGISTER(TYPE, NAME) \
    TYPE##_type[#NAME] = &component::TYPE::NAME;

// @Lua API
void set_lua_entity(sol::state &lua) {
    sol::usertype<Entity> entity_type = lua.new_usertype<Entity>("pine_Entity",
        sol::constructors<Entity(), Entity(Scene*)>());

    entity_type["remove"] = &Entity::remove;
    entity_type["disable_serialization"] = [&](Entity &self) {
        self.add_empty_component<EntityDoNotSerialize>();
    };
    entity_type["enable_serialization"] = [&](Entity &self) {
        if (self.has_component<EntityDoNotSerialize>()) [[likely]] {
            self.remove_component<EntityDoNotSerialize>();
        }
    };
    
    FUNC_REGISTER(Tag, std::string)
    FUNC_REGISTER_NA(Transform)
    FUNC_REGISTER(Script, std::string)
    FUNC_REGISTER(Table, sol::table)
    FUNC_REGISTER(CustomBehavior, std::string)
    FUNC_REGISTER(Sprite, std::string)
    FUNC_REGISTER(StateFlags, std::vector<std::string>)
}

// @Lua API
void set_lua_components(sol::state &lua) {
    COMP_REGISTER(Tag, std::string)
    COMP_MEM_REGISTER(Tag, name)

    COMP_REGISTER_NA(Transform)
    COMP_MEM_REGISTER(Transform, x)
    COMP_MEM_REGISTER(Transform, y)
    COMP_MEM_REGISTER(Transform, sx)
    COMP_MEM_REGISTER(Transform, sy)
    COMP_MEM_REGISTER(Transform, rr)
    COMP_MEM_REGISTER(Transform, set_pos)
    COMP_MEM_REGISTER(Transform, set_scale)

    COMP_REGISTER(Script, std::string)
    COMP_MEM_REGISTER(Script, push_script)
    Script_type["run"] = [](component::Script &self, sol::variadic_args va) {
        component::Script::id_t id = 0;
        if (va.size() > 0) {
            id = va.get<component::Script::id_t>();
        }
        self.run(id);
    };
    COMP_MEM_REGISTER(Script, run_all)

    COMP_REGISTER(Table, sol::table)
    COMP_MEM_REGISTER(Table, table)

    COMP_REGISTER(CustomBehavior, std::string)
    COMP_MEM_REGISTER(CustomBehavior, set_on_update)
    COMP_MEM_REGISTER(CustomBehavior, set_on_remove)

    COMP_REGISTER(Sprite, std::string)
    COMP_MEM_REGISTER(Sprite, set_texture)
    Sprite_type["set_render_layer"] = [](component::Sprite &self, float val) { self.render_layer = val; };

    COMP_REGISTER(StateFlags, std::vector<std::string>)
    COMP_MEM_REGISTER(StateFlags, set_flags)
    COMP_MEM_REGISTER(StateFlags, has_flags)
}

// @Lua API
void set_lua_scene(sol::state &lua, SceneManager &manager) {
    sol::usertype<Scene> scene_type = lua.new_usertype<Scene>("pine_Scene",
        sol::constructors<Scene(std::string)>());

    sol::usertype<Camera> camera_type = lua.new_usertype<Camera>("pine_Camera");

    scene_type["add_entity"] = [&](Scene &self, sol::variadic_args name) { 
        return sol::make_object(lua, self.add_entity(name.size() == 0 ? "" : name.get<std::string>()));
    };
    scene_type["get_entities"] = &Scene::get_entities;
    scene_type["get_close_entities"] = &Scene::get_close_entities;

    scene_type["get_camera"] = [&](Scene &self) {
        return sol::make_object(lua, self.get_camera());
    };

    /// Camera
    camera_type["get_pos"] = [&](Camera &self) {
        auto &temp = self.get_position();
        return glm::vec2{ temp.x, temp.y };
    };
    camera_type["set_pos"] = [&](Camera &self, const glm::vec2 &pos) {
        const auto &curr_pos = self.get_position();
        self.set_position({ pos.x, pos.y, curr_pos.z });
    };
    camera_type["left"] = &Camera::left;
    camera_type["right"] = &Camera::right;
    camera_type["up"] = &Camera::up;
    camera_type["down"] = &Camera::down;

    /// Manager related
    lua.set_function("pine_get_scene", [&]() {
        return sol::make_object(lua, manager.get_scene());
    });
    /// @warning this also removes the current one from memory
    lua.set_function("pine_set_scene", [&](Scene *scene) {
        manager.pop();
        auto storage_holder = std::make_unique<Scene>(*scene);
        Scene *pointer = storage_holder.get();
        manager.push(std::move(storage_holder));
        manager.try_update();
        return sol::make_object(lua, pointer);
    });
    lua.set_function("pine_set_temp_scene", [&](Scene *scene) {
        auto storage_holder = std::make_unique<Scene>(*scene);
        Scene *pointer = storage_holder.get();
        manager.push(std::move(storage_holder));
        manager.try_update();
        return sol::make_object(lua, pointer);
    });
    lua.set_function("pine_remove_temp_scene", [&]() {
        manager.pop();
        manager.try_update();
    });

    lua.set_function("pine_save_current_scene", [&]() {
       SceneSerializer::instance().save_current(manager);
    });
    lua.set_function("pine_load_scene", [&](std::string name) {
        return sol::make_object(lua, SceneSerializer::instance().load_from_name(manager, name));
    });
}