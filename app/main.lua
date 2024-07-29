Cone = require("core.Cone.cone")

-- Here this function just creates a scene with entities and stuff
-- It is called before the game begins
local function setup()
    --if pine_load_scene("main_scene") ~= nil then return end

    local my_scene = pine_set_scene(pine_new_scene("main_scene"))
    local spawner_ent = my_scene:add_entity("spawner") -- optional name for the entity

    local script_component = spawner_ent:add_component_Script()
    local spawn_script_id = script_component:push_script("scripts/spawn_script.lua")

    for i = 1, 3000 do
        script_component:run(spawn_script_id)
    end

    spawner_ent:add_component_CustomBehavior("scripts/spawner_on_update.lua")

    local coll_comp = spawner_ent:add_component_Collider()
    coll_comp:type_box()
    coll_comp.resolve_collisions = true

    local closeby_ents = my_scene:get_close_entities(spawner_ent, 10)
    for i, v in ipairs(closeby_ents) do
        v:remove()
    end
end

local function close()
    --pine_save_current_scene()
    -- Define closing logic here if needed

    -- Game saving will be here: pine_save_current_scene()
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
    close()
end