#include "events/LuaEvents.h"

#include "events/WindowEvent.h"
#include "events/MouseEvent.h"
#include "events/KeyEvent.h"
#include "events/CustomEvent.h"

// @Lua API
void set_lua_event_handlers(sol::state &lua, EventBus &bus) {
    lua.set_function("pine_set_event_handler_KeyPressed", [&](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        bus.subscribe<KeyPressedEvent>([stored_callback](KeyPressedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->key);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    lua.set_function("pine_set_event_handler_KeyRepeat", [&](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        bus.subscribe<KeyRepeatEvent>([stored_callback](KeyRepeatEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->key);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    lua.set_function("pine_set_event_handler_KeyReleased", [&](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        bus.subscribe<KeyReleasedEvent>([stored_callback](KeyReleasedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->key);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    lua.set_function("pine_set_event_handler_MouseButtonPressed", [&](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        bus.subscribe<MouseButtonPressedEvent>([stored_callback](MouseButtonPressedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->button, event->x_pos, event->y_pos);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    lua.set_function("pine_set_event_handler_MouseButtonReleased", [&](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        bus.subscribe<MouseButtonReleasedEvent>([stored_callback](MouseButtonReleasedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->button);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    lua.set_function("pine_set_event_handler_MouseMoved", [&](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        bus.subscribe<MouseMovedEvent>([stored_callback](MouseMovedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->x_pos, event->y_pos);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    lua.set_function("pine_set_event_handler_Custom", [&](const char *title, sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        bus.subscribe<CustomLuaEvent>([title, stored_callback](CustomLuaEvent *event) -> HandlerPersistence {
            if (strcmp(event->title.c_str(), title) == 0) {
                bool result = (*stored_callback)(*event->data);
                return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
            }
            return HandlerPersistence::Continuous;
        });
    });
}