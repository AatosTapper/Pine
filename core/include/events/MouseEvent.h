#pragma once

#include "Event.h"

class MouseButtonPressedEvent : public Event {
public:
    MouseButtonPressedEvent(int _button) : button(_button) {}

    EVENT_CLASS_TYPE(MouseButtonPressed)
    EVENT_CLASS_CATEGORY(MOUSE)
    CREATE_TYPE_INDEX(MouseButtonPressedEvent)

    int button = 0;
};

class MouseButtonReleasedEvent : public Event {
public:
    MouseButtonReleasedEvent(int _button) : button(_button) {}

    EVENT_CLASS_TYPE(MouseButtonReleased)
    EVENT_CLASS_CATEGORY(MOUSE)
    CREATE_TYPE_INDEX(MouseButtonReleasedEvent)

    int button = 0;
};

class MouseMovedEvent : public Event {
public:
    MouseMovedEvent(double x, double y) : x_pos(x), y_pos(y) {}

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(MOUSE)
    CREATE_TYPE_INDEX(MouseMovedEvent)

    double x_pos;
    double y_pos;
};