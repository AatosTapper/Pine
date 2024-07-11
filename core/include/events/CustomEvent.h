#pragma once

#include "Event.h"
#include "ScriptEngine.h"

class CustomLuaEvent : public Event {
public:
    CustomLuaEvent(const char *_title, std::shared_ptr<sol::object> _data) noexcept : 
        title(_title), data(_data) {}

    EVENT_CLASS_TYPE(CustomLua)
    EVENT_CLASS_CATEGORY(CUSTOM)
    CREATE_TYPE_INDEX(CustomLuaEvent)

    std::string title;
    std::shared_ptr<sol::object> data = nullptr;
};