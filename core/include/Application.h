#pragma once

#include "pch.h"

#include "ScriptEngine.h"
#include "Window.h"
#include "events/Event.h"
#include "rendering/Renderer.h"

class Application {
public:
    Application(sol::state &lua);
    ~Application();

    void entry();

    void on_event(Event *e);
    void on_window_close();
 
private:
    void m_run();
    void m_set_lua_functions();
    void m_update_logic();
    void m_render();

    void m_set_lua_event_handlers();

    
    EventBus m_bus;
    sol::state &m_lua;
    std::unique_ptr<Window> m_window = nullptr;
    std::unique_ptr<Renderer> m_renderer = nullptr;
    bool m_running = true;
};