#include "Application.h"

#include <GLFW/glfw3.h>

#include "events/WindowEvent.h"
#include "events/MouseEvent.h"
#include <chrono>

static void log_frametime(double frametime);

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
    m_window->set_event_callback([this](Event *e){ 
        this->on_event(e);
    });
    m_bus.subscribe<WindowCloseEvent>([this](WindowCloseEvent *event) {
        this->on_window_close();
    });
}

Application::~Application() {

}

void Application::on_event(Event *e) {
    m_bus.publish(e);

    std::cout << e->get_name() << std::endl;
}

void Application::on_window_close() { 
    m_running = false;
}

void Application::entry() {
    ScriptEngine::run_script(m_lua, SCRIPT("main.lua"));

    m_lua["main"]();
}

void Application::m_run() {

    m_bus.subscribe<MouseButtonPressedEvent>([](MouseButtonPressedEvent *event) {
        (void)event;
    });
    double updates_per_sec = ScriptEngine::get_config_var_double(m_lua, "updates_per_sec");

    uint32_t frame_counter = 0;
    uint32_t update_counter = 0;
    double frametime_accumulator = 0.0;
    double last_update = glfwGetTime();
    double update_time = 1.0 / updates_per_sec;
    double fps_log_accumulator = 0.0;

    while (m_running) {
        double frame_start_time = glfwGetTime();
        double delta_time = frame_start_time - last_update;
        last_update = frame_start_time;
        frametime_accumulator += delta_time;
        fps_log_accumulator += delta_time;
        frame_counter++;

        while (frametime_accumulator >= update_time)
        {
            // update logic
            update_counter++;
            frametime_accumulator -= update_time;
        }

        // render
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_window->update();
        
        // logging
        if (update_counter == static_cast<uint32_t>(updates_per_sec))
        {
            fps_log_accumulator /= static_cast<double>(frame_counter);
            log_frametime(fps_log_accumulator);
            fps_log_accumulator = 0.0;
            update_counter = 0u;
            frame_counter = 0u;
        }
    }
}

void log_frametime(double frametime) {
    std::cout << "Frametime (capped): " 
        << static_cast<uint32_t>(frametime * 1000000.0) / 1000.0f << "ms = " 
        << static_cast<uint32_t>(std::round(1.0 / frametime)) << " FPS\n";
}

void Application::m_set_lua_functions() {
    m_lua.set_function("pine_run", &Application::m_run, this);
}