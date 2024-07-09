#include "Input.h"

InputBus::InputBus(EventBus &bus) : m_event_bus(bus) {
    m_event_bus.subscribe<KeyPressedEvent>([this](KeyPressedEvent *event) {
        this->m_callback_pressed(event);
        return HandlerPersistence::Continuous;
    });
    m_event_bus.subscribe<KeyReleasedEvent>([this](KeyReleasedEvent *event) {
        this->m_callback_released(event);
        return HandlerPersistence::Continuous;
    });
}

bool InputBus::is_pressed(int key) const {
    return std::find(m_inputs.begin(), m_inputs.end(), key) != m_inputs.end();
}

void InputBus::update() {
    m_inputs.erase(std::remove_if(m_inputs.begin(), m_inputs.end(),
        [](const auto &obj) { return !obj.active; }),
    m_inputs.end());
}

void InputBus::m_callback_pressed(KeyPressedEvent *event) {
    int key = event->key;
    if (is_pressed(key)) return;
    m_inputs.push_back(Input {
        .key = key,
        .active = true
    });
}

void InputBus::m_callback_released(KeyReleasedEvent *event) {
    auto it = std::find(m_inputs.begin(), m_inputs.end(), event->key);
    if (it == m_inputs.end()) [[unlikely]] return;
    it->active = false;
}