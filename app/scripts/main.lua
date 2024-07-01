-- main() is the entry point of a Pine application

Cone = require("core.Cone.cone")

function setup()
    print("Game starting")

    mouse_callback = function(key)
        print("Lua Mouse click: ", key)
        return false
    end
    Cone.event_listener("MouseButtonPressed", mouse_callback)

    key_callback = function(key)
        print("Lua cey clicked")
        return false
    end
    Cone.event_listener("KeyPressed", key_callback)

end

function cleanup()
    print("Game closing")
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
end