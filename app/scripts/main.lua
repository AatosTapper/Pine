-- main() is the entry point of a Pine application

Cone = require("core.Cone.cone")

local mouse_button_callback = function(key, x, y)
    print("Mouse at " .. 
        Cone.Math.round_to_decimal(x, 1) .. ", " .. 
        Cone.Math.round_to_decimal(y, 1))
end

local custom_event_callback = function(data)
    data.update_func()
end

local function setup()
    Cone.Event.listener("MouseButtonPressed", mouse_button_callback)
    Cone.Event.listener("SomeGameplayEvent", custom_event_callback)

    Cone.Event.create("SomeGameplayEvent", {
        update_func = function()
            print("The event listener calls this function in the callback!")
        end
    })
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
end