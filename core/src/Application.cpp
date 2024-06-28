#include "Application.h"

#include <GLFW/glfw3.h>

#include "events/WindowEvent.h"
#include "events/MouseEvent.h"
#include "events/KeyEvent.h"

#include "FrameData.h"

Application::Application() :
    m_lua(ScriptEngine::create_lua_state())
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
    m_bus.subscribe<WindowCloseEvent>([this](WindowCloseEvent *event) -> HandlerPersistence {
        this->on_window_close();
        return HandlerPersistence::Single;
    });
}

Application::~Application() {

}

void Application::on_event(Event *e) {
    m_bus.publish(e);

    //std::cout << e->get_name() << std::endl;
}

void Application::on_window_close() { 
    m_running = false;
}

void Application::entry() {
    ScriptEngine::run_script(m_lua, SCRIPT("main.lua"));

    m_lua["main"]();
}

void Application::m_run() {
    const double updates_per_sec = ScriptEngine::get_config_var_double(m_lua, "updates_per_sec");
    FrameData frame_data(updates_per_sec);

    while (m_running) {
        frame_data.update_frame_data();

        while (frame_data.frametime_accumulator >= frame_data.update_time)
        {
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
}

void Application::m_set_lua_event_handlers() {
    m_lua.set_function("pine_set_event_handler_KeyPressed", [this](std::function<bool(int)> callback) {
        m_bus.subscribe<KeyPressedEvent>([callback](KeyPressedEvent *event) -> HandlerPersistence {
            return callback(event->key) ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_KeyRepeat", [this](std::function<bool(int)> callback) {
        m_bus.subscribe<KeyRepeatEvent>([callback](KeyRepeatEvent *event) -> HandlerPersistence {
            return callback(event->key) ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_KeyReleased", [this](std::function<bool(int)> callback) {
        m_bus.subscribe<KeyReleasedEvent>([callback](KeyReleasedEvent *event) -> HandlerPersistence {
            return callback(event->key) ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_MouseButtonPressed", [this](std::function<bool(int)> callback) {
        m_bus.subscribe<MouseButtonPressedEvent>([callback](MouseButtonPressedEvent *event) -> HandlerPersistence {
            return callback(event->button) ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_MouseButtonReleased", [this](std::function<bool(int)> callback) {
        m_bus.subscribe<MouseButtonReleasedEvent>([callback](MouseButtonReleasedEvent *event) -> HandlerPersistence {
            return callback(event->button) ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
    m_lua.set_function("pine_set_event_handler_MouseMoved", [this](std::function<bool(double, double)> callback) {
        m_bus.subscribe<MouseMovedEvent>([callback](MouseMovedEvent *event) -> HandlerPersistence {
            return callback(event->x_pos, event->y_pos) ? HandlerPersistence::Single : HandlerPersistence::Continuous;
        });
    });
}

void Application::m_update_logic() {

}

void Application::m_render() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_window->update();
}