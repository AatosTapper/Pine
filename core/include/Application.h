#pragma once

#include "pch.h"

#include "ScriptEngine.h"
#include "Window.h"
#include "events/Event.h"

class Application {
public:
    Application();
    ~Application();

    void entry();

    void on_event(Event *e);
    void on_window_close();

private:
    void m_run();
    void m_set_lua_functions();

    EventBus m_bus;
    sol::state m_lua;
    std::unique_ptr<Window> m_window = nullptr;
    bool m_running = true;
};