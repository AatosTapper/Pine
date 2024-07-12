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

    local scene = pine_set_scene(pine_Scene:new())
    local spawner_ent = scene:add_entity("spawner")
    local script = spawner_ent:add_component_Script("scripts/spawn_script.lua")
    for i = 1, 100 do
        script:run()
    end
    local table_comp = spawner_ent:add_component_Table({
        health = 10,
        xp = 100,
        name = "Kakkeli",
        abilities = { "jou", "fdsla", "saö", "asdasö" }
    })

    local camera = scene:get_camera()
    local behaviour = spawner_ent:add_component_CustomBehaviour()

    behaviour:set_on_update(function()
        -- simple controller using pine_get_input(keycode)
        local dt = pine_fixed_update_dt()
        local speed = 18 * dt
        --print(dt)
        if pine_get_input(Cone.Key._W) then
            camera:up(speed)
        end
        if pine_get_input(Cone.Key._S) then
            camera:down(speed)
        end
        if pine_get_input(Cone.Key._A) then
            camera:left(speed)
        end
        if pine_get_input(Cone.Key._D) then
            camera:right(speed)
        end
        if pine_get_input(Cone.Key._MOUSE_BUTTON_LEFT) then
            camera:left(speed)
        end
        if pine_get_input(Cone.Key._MOUSE_BUTTON_RIGHT) then
            camera:right(speed)
        end

        --local mouse_pos = pine_get_mouse_pos();
        --local pos = vec2.from(camera:get_pos())
        --camera:set_pos(vec2.from(pos.x + ((math.random() - 0.5) * 0.2), pos.y + ((math.random() - 0.5) * 0.2)))
    end)

    behaviour:set_on_remove(function()
        print("Custom Behaviour deleted")
    end)
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
    -- calling call_on_update in C++ doesn't work
end