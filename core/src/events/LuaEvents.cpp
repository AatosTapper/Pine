#include "events/LuaEvents.h"

#include "events/WindowEvent.h"
#include "events/MouseEvent.h"
#include "events/KeyEvent.h"
#include "events/CustomEvent.h"

#include "Input.h"

// @Lua API
void set_lua_event_handlers(sol::state &lua, EventBus &bus, InputBus &input) {
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
            bool result = (*stored_callback)(mousebutton_to_keycode(event->button), event->x_pos, event->y_pos);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    lua.set_function("pine_set_event_handler_MouseButtonReleased", [&](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        bus.subscribe<MouseButtonReleasedEvent>([stored_callback](MouseButtonReleasedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(mousebutton_to_keycode(event->button));
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
    lua.set_function("pine_set_event_handler_Custom", [&](std::string title, sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        bus.subscribe<CustomLuaEvent>([title, stored_callback](CustomLuaEvent *event) -> HandlerPersistence {
            if (title == event->title) {
                sol::protected_function_result result = stored_callback->call(*event->data);
                if (!result.valid()) {
                    sol::error err = result;
                    std::cerr << "Lua Error in custom event handler: " << err.what() << std::endl;
                }
                return result.get<bool>() ? HandlerPersistence::Single : HandlerPersistence::Continuous;
            }
            return HandlerPersistence::Continuous;
        });
    });
    lua.set_function("pine_create_event_Custom", [&](std::string title, sol::table data) {
        auto data_ptr = std::make_shared<sol::table>(data);
        CustomLuaEvent event(title, data_ptr);
        bus.publish(&event);
    });

    lua.set_function("pine_get_input", [&](int key) {
        return input.is_pressed(key);
    });

    lua.set_function("pine_get_mouse_pos", [&] {
        return input.get_mouse_pos();
    });
}