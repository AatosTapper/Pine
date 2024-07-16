-- main() is the entry point of a Pine application

Cone = require("core.Cone.cone")

local mouse_button_callback = function(key, x, y)
    print("Mouse at " .. 
        Cone.Math.round_to_decimal(x, 1) .. ", " .. 
        Cone.Math.round_to_decimal(y, 1))
end

local custom_event_callback = function(data)
    print(data.msg)
end

local function setup()
    Cone.Event.listener("MouseButtonPressed", mouse_button_callback)
    Cone.Event.listener("SomeGameplayEvent", custom_event_callback)
    Cone.Event.create("SomeGameplayEvent", { msg = "hello" })

    local scene = pine_set_scene(pine_Scene:new())
    local spawner_ent = scene:add_entity("spawner")
    local script = spawner_ent:add_component_Script("scripts/spawn_script.lua")
    for i = 1, 2000 do
        script:run()
    end

    local camera = scene:get_camera()
    local behaviour = spawner_ent:add_component_CustomBehaviour()

    behaviour:set_on_update("scripts/spawner_on_update.lua")
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
    -- calling call_on_update in C++ doesn't work
end