#include "ScriptEngine.h"

#include <fstream>
#include <sstream>

static std::unordered_map<std::string, sol::bytecode> script_cache;

static sol::string_view get_script(sol::state &lua, const std::string &file_path) {
    if (script_cache.find(file_path) == script_cache.end()) [[unlikely]] {
        std::ifstream file(file_path);
        if (!file) {
            std::cerr << "File load error: can't load file \"" << file_path << "\" (check the part after ../app/)\n";
            std::abort();
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        sol::load_result lr = lua.load(buffer.str());
        if (!lr.valid()) {
            std::cerr << "Lua bytecode load error: for file \"" << file_path << "\"\n";
            std::abort();
        }
        sol::protected_function target = lr.get<sol::protected_function>();
        script_cache[file_path] = target.dump();
    }
    return script_cache[file_path].as_string_view();
}

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
        sol::lib::os,
        sol::lib::ffi,
        sol::lib::jit
    );
    state.script(R"(
        package.path = package.path .. ";../?.lua"
    )");

    state.script_file("../core/Cone/pine_api.lua");

    return state;
}

sol::protected_function_result ScriptEngine::run_script(sol::state &lua, const std::string &file_path) {
    try {
        return lua.safe_script(get_script(lua, file_path));
    } catch (const sol::error &e) {
        std::cerr << "Sol2 caught error: " << e.what() << std::endl;
        std::abort();
    } catch (const std::exception &e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        std::abort();
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        std::abort();
    }
    assert(false && "wtf how tf did you get here dawg??");
}

std::string ScriptEngine::get_config_var_string(sol::state &lua, const std::string &name) {
    auto var = lua["config"][name];
    if (!var.valid()) {
        std::cout << "\n\nError: must define [" << name << "] in config.lua file\n\n" << std::endl;
        assert(false);
    }
    return var.get<std::string>();
}

double ScriptEngine::get_config_var_double(sol::state &lua, const std::string &name) {
    auto var = lua["config"][name];
    if (!var.valid()) {
        std::cout << "\n\nError: must define [" << name << "] in config.lua file\n\n" << std::endl;
        assert(false);
    }
    return var.get<double>();
}

int ScriptEngine::get_config_var_int(sol::state &lua, const std::string &name) {
    return static_cast<int>(get_config_var_double(lua, name));
}