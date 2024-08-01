#include "Application.h"
#include "ScriptEngine.h"

int main() {
    sol::state lua = ScriptEngine::create_lua_state();
    LuaStateDispatcher::instance().m_lua = &lua;
    Application app(lua);
    app.entry();
    return 0;
}