#pragma once

#include "Event.h"
#include "ScriptEngine.h"

class CustomLuaEvent : public Event {
public:
    CustomLuaEvent(const std::string &_title, std::shared_ptr<sol::table> _data) noexcept : 
        title(_title), data(_data) { assert(!title.empty()); }

    EVENT_CLASS_TYPE(CustomLua)
    EVENT_CLASS_CATEGORY(CUSTOM)
    CREATE_TYPE_INDEX(CustomLuaEvent)

    std::string title;
    std::shared_ptr<sol::table> data = nullptr;
};