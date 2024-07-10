#include "Application.h"
#include "ScriptEngine.h"

int main() {
    sol::state lua = ScriptEngine::create_lua_state();
    Application app(lua);
    app.entry();

    return 0;
}