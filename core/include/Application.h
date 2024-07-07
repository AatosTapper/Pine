#pragma once

#include "pch.h"

#include "ScriptEngine.h"
#include "Window.h"
#include "events/Event.h"
#include "rendering/Renderer.h"
#include "rendering/Camera.h"
#include "scene/SceneManager.h"

class Application {
public:
    Application(sol::state &lua);
    ~Application();

    void entry();
    void on_event(Event *e);

private:
    void m_run();
    void m_on_window_close();
    void m_set_lua_functions();
    void m_update_logic();
    void m_render();
    
    EventBus m_bus;
    SceneManager m_scene_manager;
    sol::state &m_lua;
    std::unique_ptr<Window> m_window = nullptr;
    std::unique_ptr<Renderer> m_renderer = nullptr;
    std::unique_ptr<Camera> m_camera = nullptr;
    bool m_running = true;
};