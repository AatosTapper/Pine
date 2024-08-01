--- Game specific GLOBAL stuff here
local game = {}

---@param scene pine_Scene
function game.create_player(scene)
    local player = scene:add_entity("player")
    player:add_component_CustomBehavior("on_update/player.lua")
    player:add_component_Sprite("res/textures/test.png")
    player:add_component_Table({
        velocity = { x = 0, y = 0 }
    })
end

---@param scene pine_Scene
---@param num integer
---@param width number
---@param height number
function game.create_grass(scene, spacing, width, height)
    -- Calculate the number of items that fit in the width and height based on spacing
    local num_x = math.floor(width / spacing)
    local num_y = math.floor(height / spacing)
    
    local total = 0
    for y = 1, num_y - 1 do
        for x = 1, num_x - 1 do
            local ent = scene:add_entity("comp")
            local sprite_comp = ent:add_component_Sprite("res/textures/grass.png")
            local transform = ent:get_component_Transform()
            
            -- Calculate positions with the specified spacing
            local x_pos = x * spacing - width / 2
            local y_pos = y * spacing - height / 2

            local jitter = spacing * 0.7
            
            transform:set_pos(x_pos + (math.random() * 2 - 1) * jitter, y_pos + (math.random() * 2 - 1) * jitter)
            transform:set_scale(1.4, 1.4)

            sprite_comp:set_render_layer(-height - transform.y)

            ent:add_component_CustomBehavior("on_update/grass.lua")
            ent:add_component_StateFlags({ "grass_interactable" })

            total = total + 1
        end
    end
    print(total)
end



---@param scene pine_Scene
---@param width number
---@param height number
function game.create_grass_bg(scene, width, height)
    local ent = scene:add_entity("bg")
    local sprite_comp = ent:add_component_Sprite("res/textures/grass_bg.png")
    sprite_comp:set_render_layer(-100.0)
    local transform = ent:get_component_Transform()
    transform:set_scale(width, height)
end

---@param scene pine_Scene
function game.create_camera_controller(scene, player)
    local camera_controller = scene:add_entity("camera_controller")
    camera_controller:add_component_CustomBehavior("on_update/camera_controller.lua")
    camera_controller:add_component_Table({
        velocity = { x = 0.0, y = 0.0 }
    })
end

function game.create_main_scene()
    local main_scene = pine_set_scene(pine_new_scene("main_scene"))
    game.create_camera_controller(main_scene)
    game.create_player(main_scene)
    game.create_grass(main_scene, 0.9, 60, 60)
    game.create_grass_bg(main_scene, 30, 30)
end

return game