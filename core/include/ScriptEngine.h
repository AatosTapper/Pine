#pragma once

#include "pch.h"

#include "singleton.h"

#define SCRIPT(name) "../app/" name

#define LUA_VEC(TYPE, lua) \
    lua.set_function("pine_"#TYPE"_vec", []() -> std::vector<TYPE> { return {}; })

class ScriptEngine {
public:
    IMPL_NO_COPY(ScriptEngine)

    static sol::state create_lua_state();
    static void run_script(sol::state &lua, const std::string &file_path);

    static std::string get_config_var_string(sol::state &lua, const std::string &name);
    static double get_config_var_double(sol::state &lua, const std::string &name);
    static int get_config_var_int(sol::state &lua, const std::string &name);

private:
    ScriptEngine() = delete;
};
