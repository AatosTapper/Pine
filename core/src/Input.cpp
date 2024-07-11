#include "Input.h"

#include "events/MouseEvent.h"

InputBus::InputBus(EventBus &bus) noexcept : m_event_bus(bus) {
    m_event_bus.subscribe<KeyPressedEvent>([this](KeyPressedEvent *event) {
        this->m_callback_pressed(event->key);
        return HandlerPersistence::Continuous;
    });
    m_event_bus.subscribe<KeyReleasedEvent>([this](KeyReleasedEvent *event) {
        this->m_callback_released(event->key);
        return HandlerPersistence::Continuous;
    });
    m_event_bus.subscribe<MouseButtonPressedEvent>([this](MouseButtonPressedEvent *event) {
        this->m_callback_pressed(mousebutton_to_keycode(event->button));
        return HandlerPersistence::Continuous;
    });
    m_event_bus.subscribe<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent *event) {
        this->m_callback_released(mousebutton_to_keycode(event->button));
        return HandlerPersistence::Continuous;
    });
    m_event_bus.subscribe<MouseMovedEvent>([this](MouseMovedEvent *event) {
        this->m_mouse_pos = glm::vec2(event->x_pos, event->y_pos);
        return HandlerPersistence::Continuous;
    });
}

bool InputBus::is_pressed(int key) const {
    return std::find(m_inputs.begin(), m_inputs.end(), key) != m_inputs.end();
}

const glm::vec2 &InputBus::get_mouse_pos() const {
    return m_mouse_pos;
}

void InputBus::update() {
    m_inputs.erase(std::remove_if(m_inputs.begin(), m_inputs.end(),
        [](const auto &obj) { return !obj.active; }),
    m_inputs.end());
}

constexpr void InputBus::m_callback_pressed(int key) {
    if (is_pressed(key)) return;
    m_inputs.push_back(Input {
        .key = key,
        .active = true
    });
}

constexpr void InputBus::m_callback_released(int key) {
    auto it = std::find(m_inputs.begin(), m_inputs.end(), key);
    if (it == m_inputs.end()) [[unlikely]] return;
    it->active = false;
}