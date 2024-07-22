-- main() is the entry point of a Pine application

local save = false

local function setup()
    -- THIS IS FOR GAME SAVING, CHECKS IF A SAVEFILE EXIST AND LOADS IT
    if save then
        if pine_load_scene("main_scene") ~= nil then return end
    end

    -- Creating a new scene and setting it as the current one
    local scene = pine_set_scene(pine_Scene:new("main_scene"))

    -- Adding an entity to that scene
    local spawner_ent = scene:add_entity("spawner")

    -- Adding a script component to the entity
    local script_component = spawner_ent:add_component_Script("scripts/spawn_script.lua")

    -- Running that script many times
    for i = 1, 2000 do
        script_component:run()
    end
    
    -- Adding a custom behavior component to the entity
    spawner_ent:add_component_CustomBehavior("scripts/spawner_on_update.lua")

    -- Getting the entities close to the spawner entity and deleting them
    local closeby_ents = scene:get_close_entities(spawner_ent, 10)
    for i, v in ipairs(closeby_ents) do
        v:remove()
    end
end

local function close()
    if save then
        pine_save_current_scene()
    end
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
    close()
end