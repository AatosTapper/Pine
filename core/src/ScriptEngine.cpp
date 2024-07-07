#include "ScriptEngine.h"

static void lua_panic(sol::optional<std::string> maybe_msg) {
    std::cerr << "Lua error, application will abort" << std::endl;
    if (maybe_msg) [[likely]] {
        const std::string& msg = maybe_msg.value();
        std::cerr << "\terror message: " << msg << std::endl;
    }
    std::abort();
}

sol::state ScriptEngine::create_lua_state() {
    sol::state state(sol::c_call<decltype(&lua_panic), &lua_panic>);
    state.open_libraries( // not ideal for safety
        sol::lib::base, 
        sol::lib::math, 
        sol::lib::coroutine, 
        sol::lib::string,
        sol::lib::table,
        sol::lib::package,
        sol::lib::os
    );
    state.script(R"(
        package.path = package.path .. ";../?.lua"
    )");
    return state;
}

void ScriptEngine::run_script(sol::state &lua, const std::string &file_path) {
    try {
        lua.safe_script_file(file_path);
    }
    catch (const sol::error &e) {
        std::cerr << "Sol2 caught error: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
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