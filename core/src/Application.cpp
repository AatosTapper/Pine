#include "Application.h"

#include <GLFW/glfw3.h>

#include "events/WindowEvent.h"
#include "events/MouseEvent.h"
#include "events/KeyEvent.h"
#include "events/CustomEvent.h"
#include "scene/LuaScene.h"
#include "events/LuaEvents.h"
#include "scene/System.h"
#include "LuaUtils.h"

// @Lua API
Application::Application(sol::state &lua) noexcept :
    m_input_bus(m_event_bus),
    m_lua(lua)
{
    m_set_lua_functions();
    sol::protected_function_result result = ScriptEngine::run_script(m_lua, app_relative_path("config.lua"));

    LuaStateDispatcher::instance().m_lua = &m_lua;

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

    m_camera = std::make_unique<Camera>(
        m_window->get_aspect_ratio(),
        ScriptEngine::get_config_var_double(m_lua, "cam_fov"),
        static_cast<bool>(ScriptEngine::get_config_var_int(m_lua, "cam_projection"))
    );
    m_camera->back(ScriptEngine::get_config_var_double(m_lua, "cam_start_z"));

    m_scene_manager.set_camera(m_camera.get());

    m_event_bus.subscribe<WindowResizeEvent>([this](WindowResizeEvent *event) -> HandlerPersistence {
        this->m_renderer->set_window_dimensions(glm::i32vec2(event->new_width, event->new_height));
        this->m_renderer->regenerate_framebuffer();
        this->m_camera->set_aspect_ratio(static_cast<float>(event->new_width) / static_cast<float>(event->new_height));
        return HandlerPersistence::Continuous;
    });
    m_event_bus.subscribe<WindowCloseEvent>([this](WindowCloseEvent *event) -> HandlerPersistence {
        (void)event;
        this->m_on_window_close();
        return HandlerPersistence::Single;
    });
    m_event_bus.subscribe<KeyPressedEvent>([this](KeyPressedEvent *event) -> HandlerPersistence {
        switch (event->key)
        {
        case GLFW_KEY_ESCAPE:
            this->m_on_window_close();
            break;
        }
        return HandlerPersistence::Continuous;
    });

    std::cout << "Scene size: " << sizeof(Scene) << "\n";
}

Application::~Application() {

}

void Application::on_event(Event *e) {
    m_event_bus.publish(e);
}

void Application::m_on_window_close() { 
    m_running = false;
}

void Application::entry() {
    sol::protected_function_result res = ScriptEngine::run_script(m_lua, app_relative_path("main.lua"));
    m_lua["main"]();
}

void Application::m_run() {
    assert(m_scene_manager.get_scene() && "Cannot start gameloop without a scene selected");

    const double update_rate = ScriptEngine::get_config_var_double(m_lua, "fixed_update_rate");
    const double time_step = 1.0 / update_rate;
    
    m_fixed_update_dt = time_step;

    double accumulator = 0.0;
    double frame_start = glfwGetTime();
    while (m_running) {
        const double now = glfwGetTime();
        const double dt = now - frame_start;
        frame_start = now;
        m_per_frame_dt = dt;
        std::cout << "FPS: " << 1.0 / dt << "\n";

        accumulator += dt;

        while (accumulator >= time_step) {
            m_fixed_update();
            accumulator -= time_step;
            m_frame_index++;
        }

        const float alpha = accumulator / time_step;

        m_fluid_update(alpha);
        m_update_render();
    }
}

// @Lua API
void Application::m_set_lua_functions() {
    m_lua.set_function("pine_run", &Application::m_run, this);
    m_lua.set_function("pine_frame_time", [this] { return this->m_per_frame_dt; });
    m_lua.set_function("pine_fixed_update_dt", [this] { return this->m_fixed_update_dt; });
    m_lua.set_function("pine_frame_index", [this] { return this->m_frame_index; });

    set_lua_utils(m_lua);
    set_lua_event_handlers(m_lua, m_event_bus, m_input_bus);
    set_lua_entity(m_lua);
    set_lua_components(m_lua);
    set_lua_scene(m_lua, m_scene_manager);
}

void Application::m_fixed_update() {
    m_camera->update_last_pos();

    m_update_systems();
}

void Application::m_fluid_update(float alpha) {
    m_input_bus.update();

    m_camera->update(alpha);
    m_renderer->set_view_proj_matrix(m_camera->get_vp_matrix());
}


void Application::m_update_render() {
    m_renderer->start_frame();
    m_renderer->draw_frame(m_scene_manager.get_scene());
    m_window->update();
}

void Application::m_update_systems() {
    CustomBehaviourSystem::instance().update(m_scene_manager.get_scene());
}