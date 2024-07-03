#include "Application.h"

#include <GLFW/glfw3.h>

#include "events/WindowEvent.h"
#include "events/MouseEvent.h"
#include "events/KeyEvent.h"
#include "events/CustomEvent.h"
#include "scene/LuaScene.h"

#include "FrameData.h"

Application::Application(sol::state &lua) :
    m_lua(lua)
{
    m_set_lua_functions();
    ScriptEngine::run_script(m_lua, SCRIPT("config.lua"));

    m_window = std::make_unique<Window>(
        ScriptEngine::get_config_var_int(m_lua, "window_width"), 
        ScriptEngine::get_config_var_int(m_lua, "window_height"),
        ScriptEngine::get_config_var_string(m_lua, "window_name").c_str()
    );
    m_window->set_event_callback([this](Event *e) -> HandlerPersistence { 
        this->on_event(e);
        return HandlerPersistence::Continuous;
    });
    m_renderer = std::make_unique<Renderer>();
    m_renderer->set_window_dimensions(m_window->get_dimensions());
    m_renderer->init();

    m_bus.subscribe<WindowResizeEvent>([this](WindowResizeEvent *event) -> HandlerPersistence {
        this->m_renderer->set_window_dimensions(glm::i32vec2(event->new_width, event->new_height));
        this->m_renderer->regenerate_framebuffer();
        return HandlerPersistence::Continuous;
    });
    m_bus.subscribe<WindowCloseEvent>([this](WindowCloseEvent *event) -> HandlerPersistence {
        (void)event;
        this->m_on_window_close();
        return HandlerPersistence::Single;
    });
    m_bus.subscribe<KeyPressedEvent>([this](KeyPressedEvent *event) -> HandlerPersistence {
        switch (event->key)
        {
        case GLFW_KEY_ESCAPE:
            this->m_on_window_close();
            break;
        }
        return HandlerPersistence::Continuous;
    });
}

Application::~Application() {

}

void Application::on_event(Event *e) {
    m_bus.publish(e);
}

void Application::m_on_window_close() { 
    m_running = false;
}

void Application::entry() {
    ScriptEngine::run_script(m_lua, SCRIPT("main.lua"));

    m_lua["main"]();
}

void Application::m_run() {
    assert(m_scene_manager.get_scene() && "Cannot start gameloop without a scene selected");

    FrameData frame_data(ScriptEngine::get_config_var_double(m_lua, "updates_per_sec"));
    while (m_running) {
        frame_data.update_frame_data();
        while (frame_data.frametime_accumulator >= frame_data.update_time){
            m_update_logic();
            frame_data.update_counter++;
            frame_data.frametime_accumulator -= frame_data.update_time;
        }
        m_render();
        frame_data.log_frame();
    }
}

void Application::m_set_lua_functions() {
    m_lua.set_function("pine_run", &Application::m_run, this);
    m_set_lua_event_handlers();

    m_lua.set_function("pine_create_event_Custom", [this](const char *title, sol::object data) {
        auto data_ptr = std::make_shared<sol::object>(data);
        CustomLuaEvent event(title, data_ptr);
        this->m_bus.publish(&event);
    });

    set_lua_entity(m_lua);
    set_lua_components(m_lua);
    set_lua_scene(m_lua, m_scene_manager);
}

void Application::m_set_lua_event_handlers() {
    m_lua.set_function("pine_set_event_handler_KeyPressed", [this](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        m_bus.subscribe<KeyPressedEvent>([stored_callback](KeyPressedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->key);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_KeyRepeat", [this](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        m_bus.subscribe<KeyRepeatEvent>([stored_callback](KeyRepeatEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->key);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_KeyReleased", [this](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        m_bus.subscribe<KeyReleasedEvent>([stored_callback](KeyReleasedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->key);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_MouseButtonPressed", [this](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        m_bus.subscribe<MouseButtonPressedEvent>([stored_callback](MouseButtonPressedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->button, event->x_pos, event->y_pos);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_MouseButtonReleased", [this](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        m_bus.subscribe<MouseButtonReleasedEvent>([stored_callback](MouseButtonReleasedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->button);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_MouseMoved", [this](sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        m_bus.subscribe<MouseMovedEvent>([stored_callback](MouseMovedEvent *event) -> HandlerPersistence {
            bool result = (*stored_callback)(event->x_pos, event->y_pos);
            return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_Custom", [this](const char *title, sol::function callback) {
        auto stored_callback = std::make_shared<sol::function>(callback);
        m_bus.subscribe<CustomLuaEvent>([title, stored_callback](CustomLuaEvent *event) -> HandlerPersistence {
            if (strcmp(event->title.c_str(), title) == 0) {
                bool result = (*stored_callback)(*event->data);
                return result ? HandlerPersistence::Single : HandlerPersistence::Continuous;
            }
            return HandlerPersistence::Continuous;
        });
    });
}

void Application::m_update_logic() {

}

void Application::m_render() {
    m_renderer->start_frame();
    m_renderer->draw_frame();
    m_window->update();
}