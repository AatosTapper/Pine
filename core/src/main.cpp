#include "Application.h"
#include "ScriptEngine.h"
#include "scene/Scene.h"
#include "scene/Entity.h"
#include "scene/Components.h"

int main() {
    sol::state lua = ScriptEngine::create_lua_state();
    Application app(lua);
    app.entry();

    return 0;
}