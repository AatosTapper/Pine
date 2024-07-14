#include "Application.h"
#include "ScriptEngine.h"

#include "FromString.h"

#include "SceneParser.h"

int main() {
    auto scene = read_scene(app_relative_path("scenes/scene.xml").c_str());
    write_scene(scene, app_relative_path("scenes/scene.xml").c_str());

    sol::state lua = ScriptEngine::create_lua_state();
    Application app(lua);
    app.entry();

    return 0;
}