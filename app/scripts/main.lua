-- main() is the entry point of a Pine application

Cone = require("core.Cone.cone")

function setup()
    print("Game starting")

    local mouse_callback = function(key)
        print("Lua Mouse click: ", key)
        return false
    end
    Cone.event_listener("MouseButtonPressed", mouse_callback)

    local esc_callback = function(key)
        if key == 256 then 
            print("Lua Escape click")
        end
        return false
    end
    Cone.event_listener("KeyPressed", esc_callback)
end

function cleanup()
    print("Game closing")
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
end