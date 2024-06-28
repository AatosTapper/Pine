-- File is a part of Cone, the standard lua library of Pine
Cone = {}

--[[    enum

MyEnum = Cone.enum { "Key1", "Key2", ... }
some_var = MyEnum.Key1
]]
Cone.enum = function(keys)
    local Enum = {}
    for _, value in ipairs(keys) do
        Enum[value] = {}
    end
    return Enum
end

--[[    switch

Cone.switch(name)
  .case("Kate", function() print("This player's name rhymes with Fate")end)
  .case("Tod", function() print("This player's name rhymes with Cod") end)
  .default(function() print("This player's name is not Kate or Tod") end)
  .process()
]]
Cone.switch = function(element)
    local Table = {
        ["Value"] = element,
        ["DefaultFunction"] = nil,
        ["Functions"] = {}
    }
    
    Table.case = function(testElement, callback)
        Table.Functions[testElement] = callback
        return Table
    end
    
    Table.default = function(callback)
        Table.DefaultFunction = callback
        return Table
    end
    
    Table.process = function()
        local Case = Table.Functions[Table.Value]
        if Case then
            Case()
        elseif Table.DefaultFunction then
            Table.DefaultFunction()
        end
    end
    
    return Table
end

-- callback: function(args specified next to cases) -> bool (true == remove listener after one event)
Cone.event_listener = function(event, callback)
    Cone.switch(event)
    .case("KeyPressed", function()
        pine_set_event_handler_KeyPressed(callback)          -- key: int
    end)
    .case("KeyReleased", function()
        pine_set_event_handler_KeyRepeat(callback)           -- key: int
    end)
    .case("KeyRepeat", function()
        pine_set_event_handler_KeyReleased(callback)         -- key: int
    end)
    .case("MouseButtonPressed", function()
        pine_set_event_handler_MouseButtonPressed(callback)  -- key: int
    end)
    .case("MouseButtonReleased", function()
        pine_set_event_handler_MouseButtonReleased(callback) -- key: int
    end)
    .case("MouseMoved", function()
        pine_set_event_handler_MouseMoved(callback)          -- x and y pos: double, double
    end)
    .default(function()
        print("invalid event name")
    end)
    .process()
end

return Cone