-- main() is the entry point of a Pine application

local function setup()

    if pine_load_scene("main_scene") ~= nil then return end

    local scene = pine_set_scene(pine_Scene:new("main_scene"))
    local spawner_ent = scene:add_entity("spawner")
    local script = spawner_ent:add_component_Script("scripts/spawn_script.lua")
    for i = 1, 1500 do script:run() end
    local behaviour = spawner_ent:add_component_CustomBehavior()
    behaviour:set_on_update("scripts/spawner_on_update.lua")
end

local function close() 
    pine_save_current_scene()
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
    close()
end