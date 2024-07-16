#pragma once

#include "pch.h"

#include "singleton.h"

inline constexpr char application_path[] = "../app/";
inline constexpr std::string application_path_string() { return std::string(application_path); }
inline constexpr std::string app_relative_path(const char *path) { return application_path_string() + std::string(path); }
inline constexpr std::string app_relative_path(const std::string &path) { return application_path_string() + path; }


class LuaStateDispatcher {
    friend class Application;
public:
    IMPL_NO_COPY(LuaStateDispatcher)
    IMPL_SINGLETON_DISPATCHER(LuaStateDispatcher)

    sol::state &get_lua()  { assert(m_lua); return *m_lua; }

private:
    LuaStateDispatcher() = default;
    sol::state *m_lua = nullptr;
};

class ScriptEngine {
public:
    IMPL_NO_COPY(ScriptEngine)

    static sol::state create_lua_state();
    static sol::protected_function_result run_script(sol::state &lua, const std::string &file_path);

    static std::string get_config_var_string(sol::state &lua, const std::string &name);
    static double get_config_var_double(sol::state &lua, const std::string &name);
    static int get_config_var_int(sol::state &lua, const std::string &name);

private:
    ScriptEngine() = delete;
};

#define LUA_VEC(TYPE, lua) \
    lua.set_function("pine_"#TYPE"_vec", []() -> std::vector<TYPE> { return {}; })
