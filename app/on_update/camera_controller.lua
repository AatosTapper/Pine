
-- Goofy ahh spring simulation for the camera

local scene = pine_get_scene()
local parent = pine_get_script_parent_entity()

-- YOU SHOULD ALWAYS CHECK IF get_entity() RETURNED A VALID ENTITY
local player = scene:get_entity("player")
if player == nil then
    print("player was nil")
    return
end
local camera = scene:get_camera()
local player_transform =  player:get_component_Transform()
local table_comp = parent:get_component_Table()

local spring_speed = 0.1
local damp = 0.35

-- Calculating the spring acceleration and velocity
local player_pos = vec2.new(player_transform.x, player_transform.y)
local delta_vec = player_pos - camera:get_pos()
local direction = vec2.normalize(delta_vec)
local acceleration = direction * vec2.length(delta_vec * 0.8)^2 * spring_speed
local velocity = vec2.new(table_comp.table.velocity.x, table_comp.table.velocity.y) + acceleration

-- Updating the stored velocity and damping it
table_comp.table.velocity.x = velocity.x * damp
table_comp.table.velocity.y = velocity.y * damp


camera:set_pos(camera:get_pos() + velocity)