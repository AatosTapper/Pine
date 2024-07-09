#pragma once

#include "pch.h"
#include "events/Event.h"
#include "events/KeyEvent.h"

struct Input {
    int key = 0;
    bool active = true;

    operator int() { return key; }
    bool operator == (const int &oth) const { return key == oth; }
};

/// @brief Listens to the event bus and converts keyboard events into a list of currently pressed keys
class InputBus {
public:
    InputBus(EventBus &bus);

    bool is_pressed(int key) const;
    void update();

private:
    std::vector<Input> m_inputs;
    EventBus &m_event_bus;

    void m_callback_pressed(KeyPressedEvent *event);
    void m_callback_released(KeyReleasedEvent *event);
};