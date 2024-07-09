#include "scene/LuaScene.h"

#include "pch.h"
#include "scene/Scene.h"
#include "scene/Entity.h"
#include "scene/Components.h"
#include "scene/SceneManager.h"

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

#define COMP_REGISTER(TYPE) \
    sol::usertype<component::TYPE> TYPE##_type = lua.new_usertype<component::TYPE>("pine_comp_"#TYPE, \
        sol::constructors<component::TYPE()>());
#define COMP_REGISTER_ARGS(TYPE, ARGS) \
    sol::usertype<component::TYPE> TYPE##_type = lua.new_usertype<component::TYPE>("pine_comp_"#TYPE, \
        sol::constructors<component::TYPE(), component::TYPE(ARGS)>());

#define COMP_MEM_REGISTER(TYPE, NAME) \
    TYPE##_type[#NAME] = &component::TYPE::NAME; \

// @Lua API
void set_lua_entity(sol::state &lua) {
    sol::usertype<Entity> entity_type = lua.new_usertype<Entity>("pine_Entity",
        sol::constructors<Entity(), Entity(Scene*)>());

    entity_type["remove"] = &Entity::remove;

    FUNC_REGISTER(Tag, std::string)
    FUNC_REGISTER_NA(Transform)
    FUNC_REGISTER(Script, std::string)
    FUNC_REGISTER(Table, sol::table)
    FUNC_REGISTER(CustomBehaviour, sol::function)
    FUNC_REGISTER(Sprite, std::string)
}

// @Lua API
void set_lua_components(sol::state &lua) {
    COMP_REGISTER_ARGS(Tag, std::string)
    COMP_MEM_REGISTER(Tag, name)

    COMP_REGISTER(Transform)
    COMP_MEM_REGISTER(Transform, x)
    COMP_MEM_REGISTER(Transform, y)
    COMP_MEM_REGISTER(Transform, sx)
    COMP_MEM_REGISTER(Transform, sy)
    COMP_MEM_REGISTER(Transform, set_pos)
    COMP_MEM_REGISTER(Transform, set_scale)

    COMP_REGISTER_ARGS(Script, std::string)
    COMP_MEM_REGISTER(Script, push_script)
    Script_type["run"] = [&lua](component::Script &self, sol::variadic_args va) {
        component::Script::id_t id = 0;
        if (va.size() > 0) {
            id = va.get<component::Script::id_t>();
        }
        self.run(lua, id);
    };

    COMP_REGISTER_ARGS(Table, sol::table)
    COMP_MEM_REGISTER(Table, data)

    COMP_REGISTER_ARGS(CustomBehaviour, sol::function)
    COMP_MEM_REGISTER(CustomBehaviour, set_on_update)
    COMP_MEM_REGISTER(CustomBehaviour, set_on_remove)

    COMP_REGISTER_ARGS(Sprite, std::string)
    COMP_MEM_REGISTER(Sprite, set_texture)
}

// @Lua API
void set_lua_scene(sol::state &lua, SceneManager &manager) {
    sol::usertype<Scene> scene_type = lua.new_usertype<Scene>("pine_Scene",
        sol::constructors<Scene()>());

    sol::usertype<Camera> camera_type = lua.new_usertype<Camera>("pine_Camera");

    scene_type["add_entity"] = [&](Scene &self, sol::variadic_args name) { 
        return sol::make_object(lua, self.add_entity(name.size() == 0 ? "" : name.get<std::string>()));
    };
    scene_type["get_entities"] = [&](Scene &self) {
        return self.get_entities();
    };
    scene_type["get_camera"] = [&](Scene &self) {
        return sol::make_object(lua, self.get_camera());
    };


    //const glm::vec3 &get_position()
    //void set_position(const glm::vec3 &new_pos)

    // move
    camera_type["forward"] = &Camera::forward;
    camera_type["back"] = &Camera::back;
    camera_type["left"] = &Camera::left;
    camera_type["right"] = &Camera::right;
    camera_type["up"] = &Camera::up;
    camera_type["down"] = &Camera::down;

    // manager related
    lua.set_function("pine_get_scene", [&]() {
        return sol::make_object(lua, manager.get_scene());
    });
    /// @warning this also removes the current one from memory
    lua.set_function("pine_set_scene", [&](Scene *scene) {
        manager.pop();
        manager.push(std::make_unique<Scene>(*scene));
        return sol::make_object(lua, manager.get_scene());
    });
    lua.set_function("pine_set_temp_scene", [&](Scene scene) {
        manager.push(std::make_unique<Scene>(scene));
    });
    lua.set_function("pine_remove_temp_scene", [&]() {
        manager.pop();
    });
}