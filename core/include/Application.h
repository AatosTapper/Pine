#pragma once

#include "pch.h"

#include "ScriptEngine.h"
#include "Window.h"
#include "events/Event.h"
#include "rendering/Renderer.h"
#include "rendering/Camera.h"
#include "scene/SceneManager.h"
#include "Input.h"

class Application {
public:
    Application(sol::state &lua) noexcept;
    ~Application();

    void entry();
    void register_event(Event *e);

private:
    void m_run();
    void m_on_window_close();
    void m_init_window();
    void m_init_renderer();
    void m_set_default_handlers();
    void m_set_lua_functions();
    void m_fixed_update();
    void m_fluid_update(float alpha);
    void m_update_render();
    
    EventBus m_event_bus;
    InputBus m_input_bus;
    SceneManager m_scene_manager;
    sol::state &m_lua;

    std::unique_ptr<Window> m_window = nullptr;
    std::unique_ptr<Renderer> m_renderer = nullptr;

    double m_per_frame_dt = 1.0 / 60.0;
    double m_tick_dt = 1.0 / 60.0;
    double m_current_time = 0.0;
    uint32_t m_tick_reference = 60;
    uint64_t m_frame_index = 0;
    bool m_running = true;
};