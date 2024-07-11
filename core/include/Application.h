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
    void on_event(Event *e);

private:
    void m_run();
    void m_on_window_close();
    void m_set_lua_functions();
    void m_fixed_update();
    void m_fluid_update(float alpha);
    void m_update_render();
    void m_update_systems();
    
    EventBus m_event_bus;
    InputBus m_input_bus;
    SceneManager m_scene_manager;
    sol::state &m_lua;
    std::unique_ptr<Window> m_window = nullptr;
    std::unique_ptr<Renderer> m_renderer = nullptr;
    std::unique_ptr<Camera> m_camera = nullptr;
    double m_per_frame_dt = 1.0 / 60.0;
    double m_fixed_update_dt = 1.0 / 60.0;
    u_int64_t m_frame_index = 0;
    bool m_running = true;
};