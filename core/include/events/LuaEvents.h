#include "ScriptEngine.h"
#include "events/Event.h"

void set_lua_event_handlers(sol::state &lua, EventBus &bus);