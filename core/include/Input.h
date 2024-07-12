#pragma once

#include "pch.h"
#include "events/Event.h"
#include "events/KeyEvent.h"
#include <glm/glm.hpp>

/// @brief Make mousebutton keycodes work with normal keycodes
///        (as a continuous list and not two separate ones)
inline constexpr int mousebutton_to_keycode(int button) { return button + 350; }
inline constexpr int keycode_to_mousebutton(int button) { return button - 350; }
inline constexpr bool is_mouse_button(int keycode) { return 0 <= keycode_to_mousebutton(keycode); }

struct Input {
    int key = 0;
    bool active = true;

    constexpr operator int() { return key; }
    constexpr bool operator == (const int &oth) const { return key == oth; }
};

/// @brief Listens to the event bus and converts input events to simpler state
class InputBus {
public:
    InputBus(EventBus &bus) noexcept;

    bool is_pressed(int key) const;
    const glm::vec2 &get_mouse_pos() const;

    void update();

private:
    std::vector<Input> m_inputs;
    EventBus &m_event_bus;
    glm::vec2 m_mouse_pos{};

    void m_callback_pressed(int key);
    void m_callback_released(int key);
};