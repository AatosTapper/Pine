Cone = require("core.Cone.cone")

---@param scene pine_Scene
local function create_player(scene)
    local player = scene:add_entity("player")
    player:add_component_CustomBehavior("on_update/player.lua")
    player:add_component_Sprite("res/textures/test.png")
    player:add_component_Table({
        velocity = { x = 0, y = 0 }
    })
end

---@param scene pine_Scene
local function create_apples(scene)
    for i = 1, 50 do
        local ent = scene:add_entity("comp")
        local sprite_comp = ent:add_component_Sprite("res/textures/apple.png")
        sprite_comp:set_render_layer(-10.0)
        local transform = ent:get_component_Transform()
        transform:set_pos(math.random() * 60 - 30, math.random() * 60 - 30)
        transform:set_scale(1.3, 1.3)
    end
end

---@param scene pine_Scene
local function create_camera_controller(scene, player)
    local camera_controller = scene:add_entity("camera_controller")
    camera_controller:add_component_CustomBehavior("on_update/camera_controller.lua")
    camera_controller:add_component_Table({
        velocity = { x = 0.0, y = 0.0 }
    })
end

local function create_main_scene()
    local main_scene = pine_set_scene(pine_new_scene("main_scene"))
    create_camera_controller(main_scene)
    create_player(main_scene)
    create_apples(main_scene)
end

local function setup()
    create_main_scene()
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
end