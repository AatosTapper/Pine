#pragma once

#include "Event.h"

class WindowResizeEvent : public Event {
public:
    constexpr WindowResizeEvent(int width, int heigth) : new_width(width), new_height(heigth) {}

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(WINDOW)
    CREATE_TYPE_INDEX(WindowResizeEvent)

    int new_width = 0;
    int new_height = 0;
};

class WindowCloseEvent : public Event {
    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(WINDOW)
    CREATE_TYPE_INDEX(WindowCloseEvent)
};