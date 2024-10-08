#pragma once

#include "Event.h"

class KeyPressedEvent : public Event {
public:
    constexpr KeyPressedEvent(int _key) noexcept : key(_key) {}

    EVENT_CLASS_TYPE(KeyPressed)
    EVENT_CLASS_CATEGORY(KEYBOARD)
    CREATE_TYPE_INDEX(KeyPressedEvent)

    int key = 0;
};

class KeyReleasedEvent : public Event {
public:
    constexpr KeyReleasedEvent(int _key) noexcept : key(_key) {}

    EVENT_CLASS_TYPE(KeyReleased)
    EVENT_CLASS_CATEGORY(KEYBOARD)
    CREATE_TYPE_INDEX(KeyReleasedEvent)

    int key = 0;
};

class KeyRepeatEvent : public Event {
public:
    constexpr KeyRepeatEvent(int _key) noexcept : key(_key) {}

    EVENT_CLASS_TYPE(KeyRepeat)
    EVENT_CLASS_CATEGORY(KEYBOARD)
    CREATE_TYPE_INDEX(KeyRepeatEvent)

    int key = 0;
};