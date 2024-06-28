#include "ScriptEngine.h"

sol::state ScriptEngine::create_lua_state() {
    sol::state state;
    state.open_libraries( // not ideal for safety
        sol::lib::base, 
        sol::lib::math, 
        sol::lib::coroutine, 
        sol::lib::string,
        sol::lib::table,
        sol::lib::package
    );
    state.script(R"(
        package.path = package.path .. ";../?.lua"
    )");
    return state;
}

void ScriptEngine::run_script(sol::state &lua, const std::string &file_path) {
    try {
        lua.script_file(file_path);
    }
    catch (const sol::error &e) {
        std::cerr << e.what() << "\n\nClosing application with assert" << std::endl;
        assert(false);
    }
}

std::string ScriptEngine::get_config_var_string(sol::state &lua, const std::string &name) {
    auto var = lua["config"][name];
    if (!var.valid()) {
        std::cout << "\n\nError: must define [" << name << "] in config.lua file\n\n" << std::endl;
        assert(false);
    }
    return static_cast<std::string>(var);
}

double ScriptEngine::get_config_var_double(sol::state &lua, const std::string &name) {
    auto var = lua["config"][name];
    if (!var.valid()) {
        std::cout << "\n\nError: must define [" << name << "] in config.lua file\n\n" << std::endl;
        assert(false);
    }
    return static_cast<double>(var);
}

int ScriptEngine::get_config_var_int(sol::state &lua, const std::string &name) {
    return static_cast<int>(get_config_var_double(lua, name));
}