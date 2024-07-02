Event = {}
-- callback: function(args specified next to cases) -> bool (true == remove listener after one event)
Event.listener = function(event, callback)
    local safe_func = function(...)
        local result = callback(...)
        if result == nil then
            return false
        end
        return result
    end
    Cone.switch(event)
    .case("KeyPressed", function()
        pine_set_event_handler_KeyPressed(safe_func)          -- key: int
    end)
    .case("KeyReleased", function()
        pine_set_event_handler_KeyRepeat(safe_func)           -- key: int
    end)
    .case("KeyRepeat", function()
        pine_set_event_handler_KeyReleased(safe_func)         -- key: int
    end)
    .case("MouseButtonPressed", function()
        pine_set_event_handler_MouseButtonPressed(safe_func)  -- key: int, pos: double, double
    end)
    .case("MouseButtonReleased", function()
        pine_set_event_handler_MouseButtonReleased(safe_func) -- key: int
    end)
    .case("MouseMoved", function()
        pine_set_event_handler_MouseMoved(safe_func)          -- x and y pos: double, double
    end)
    .default(function()
        pine_set_event_handler_Custom(event, safe_func)      -- data: table
    end)
    .process()
end

Event.create = function(title, data)
    pine_create_event_Custom(title, data)
end

return Event