Cone = require("core.Cone.cone")

--- LOOK BELOW TO THE MAIN FUNCTION, IT IS THE ENTRY POINT

-- Here this function just creates a scene with entities and stuff
-- It is called before the game begins
local function setup()
    -- Creating a new scene
    local my_scene = pine_new_scene("main_scene")
    -- Setting the scene to be the currently active one
    pine_set_scene(my_scene)
    -- Internally Pine has a SceneManager that keeps track of the current scene
    -- This makes lua state handling much easier (and more performant) but less explicit

    -- The currently active scene will be updated every tick

    -- Adding an entity to that scene
    local spawner_ent = my_scene:add_entity("spawner") -- optional name for the entity
    -- name can be accessed from the Tag component: spawner_ent:get_component_Tag().name

    -- Adding a script component to the entity
    local script_component = spawner_ent:add_component_Script()
    -- Attaching a script to the script component, this returns an ID that we can use to call it
    local spawn_script_id = script_component:push_script("scripts/spawn_script.lua")

    -- Running that script many times to spawn 2000 apples, HAA!!!
    for i = 1, 2000 do
        script_component:run(spawn_script_id)
    end
    
    -- Adding a custom behavior component to the entity
    -- This defines a script that will be run every tick (conseptually every frame)
    spawner_ent:add_component_CustomBehavior("scripts/spawner_on_update.lua")


    -- Getting the entities close to the spawner entity and deleting them
    -- Because why the f*ck not
    local closeby_ents = my_scene:get_close_entities(spawner_ent, 10)
    for i, v in ipairs(closeby_ents) do
        v:remove()
    end
end

local function close()
    -- Define closing logic here if needed

    -- Game saving will be here: pine_save_current_scene()
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
    close()
end