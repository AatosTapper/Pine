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
#include "scene/SceneSerializer.h"
#include "TickRateScaling.h"


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

    m_scene_manager.set_camera_data(CameraData {
        .aspect_ratio = m_window->get_aspect_ratio(),
        .field_of_view = (float)ScriptEngine::get_config_var_double(m_lua, "cam_fov"),
        .start_z = (float)ScriptEngine::get_config_var_double(m_lua, "cam_start_z"),
        .ortho = (bool)ScriptEngine::get_config_var_int(m_lua, "cam_projection")
    });

    m_event_bus.subscribe<WindowResizeEvent>([this](WindowResizeEvent *event) -> HandlerPersistence {
        this->m_renderer->set_window_dimensions(this->m_window->get_framebuffer_dimensions());
        this->m_renderer->regenerate_framebuffer();
        this->m_scene_manager.cam_aspect_ratio_callback((float)event->new_width / (float)event->new_height);
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
    m_scene_manager.update();
    m_scene_manager.m_restrict_updates = true;

    assert(m_scene_manager.get_scene() && "Cannot start gameloop without a scene selected");

    const uint32_t tick_rate_downscale_step = ScriptEngine::get_config_var_int(m_lua, "tick_rate_downscale_step");
    const uint32_t tick_rate_target = ScriptEngine::get_config_var_int(m_lua, "tick_rate");
    m_tick_dt = 1.0 / tick_rate_target;
    uint32_t current_tick_rate = tick_rate_target;

    double accumulator = 0.0;
    double frame_start = glfwGetTime();
    while (m_running) {
        const double now = glfwGetTime();
        const double dt = now - frame_start;
        frame_start = now;
        m_per_frame_dt = dt;

        accumulate_dt_buffer(dt);
        
        accumulator += dt;
        while (accumulator >= m_tick_dt) {
            accumulator -= m_tick_dt;
            m_frame_index++;

            m_fixed_update();
            if (tick_rate_downscale_step != 0) [[likely]] {
                m_tick_dt = manage_tick_rate(current_tick_rate, tick_rate_target, tick_rate_downscale_step);
            }
        }

        const float alpha = accumulator / m_tick_dt;

        m_fluid_update(alpha);
        m_update_render();
    }
}

// @Lua API
void Application::m_set_lua_functions() {
    m_lua.set_function("pine_run", &Application::m_run, this);
    m_lua.set_function("pine_frame_time", [this] { return this->m_per_frame_dt; });
    m_lua.set_function("pine_tick_dt", [this] { return this->m_tick_dt; });
    m_lua.set_function("pine_frame_index", [this] { return this->m_frame_index; });

    set_lua_utils(m_lua);
    set_lua_event_handlers(m_lua, m_event_bus, m_input_bus);
    set_lua_entity(m_lua);
    set_lua_components(m_lua);
    set_lua_scene(m_lua, m_scene_manager);
}

void Application::m_fixed_update() {
    m_scene_manager.update();
    m_scene_manager.get_scene()->get_camera()->update_last_pos();
    update_transform_component_last_states(m_scene_manager.get_scene());

    m_update_systems();
}

void Application::m_fluid_update(float alpha) {
    m_input_bus.update();

    interpolate_transform_components(m_scene_manager.get_scene(), alpha);
    m_scene_manager.get_scene()->get_camera()->update(alpha);
    m_renderer->set_view_proj_matrix(m_scene_manager.get_scene()->get_camera()->get_vp_matrix());
}


void Application::m_update_render() {
    m_renderer->start_frame();
    m_renderer->draw_frame(m_scene_manager.get_scene());
    m_window->update();
}

void Application::m_update_systems() {
    custom_behaviour_system_update(m_scene_manager.get_scene());
}