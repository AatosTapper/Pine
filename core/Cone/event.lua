Event = {}

--- Call a function when an event (engine or custom) occurs
--- The function needs to take in argumets specific to the event it listens to.
--- For example, KeyPressed event callback looks like this: function(key: number) and MouseMoved like this function(x: number, y: number)
---@param event string
---@param callback function(any... specific to event)
Event.listener = function(event, callback)
    local title = tostring(event)
    local safe_func = function(...)
        local result = callback(...)
        if result == nil then
            return false
        end
        return result
    end                                     --- Below are the input types for different event type callbacks
    if title == "KeyPressed" then
        pine_set_event_handler_KeyPressed(safe_func)          ---@param key number
    elseif title == "KeyReleased" then
        pine_set_event_handler_KeyRepeat(safe_func)           ---@param key number
    elseif title == "KeyRepeat" then
        pine_set_event_handler_KeyReleased(safe_func)         ---@param key number
    elseif title == "MouseButtonPressed" then
        pine_set_event_handler_MouseButtonPressed(safe_func)  ---@param key number 
                                                              ---@param pos number, number
    elseif title == "MouseButtonReleased" then
        pine_set_event_handler_MouseButtonReleased(safe_func) ---@param key number
    elseif title == "MouseMoved" then
        pine_set_event_handler_MouseMoved(safe_func)          ---@param pos number, number
    else
        pine_set_event_handler_Custom(title, safe_func)      ---@param data table
    end
end

--- Dispatch a custom event. This event can be listened to with it's title
---@param title string
---@param data table
Event.create = function(title, data)
    pine_create_event_Custom(title, data)
end

return Event