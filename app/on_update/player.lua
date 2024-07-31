
local parent = pine_get_script_parent_entity()
local scene = pine_get_scene()
local transform = parent:get_component_Transform()
local velocity = parent:get_component_Table().table.velocity

local speed = 1.3 * pine_tick_dt()
local frame_movement = vec2.new(0.0, 0.0)

if pine_get_input(Cone.Key._A) then
    frame_movement.x = frame_movement.x - 1.0
end
if pine_get_input(Cone.Key._D) then
    frame_movement.x = frame_movement.x + 1.0
end
if pine_get_input(Cone.Key._W) then
    frame_movement.y = frame_movement.y + 1.0
end
if pine_get_input(Cone.Key._S) then
    frame_movement.y = frame_movement.y - 1.0
end

local acceleration = frame_movement:normalize() * speed
velocity.x = velocity.x + acceleration.x
velocity.y = velocity.y + acceleration.y

transform.x = transform.x + velocity.x
transform.y = transform.y + velocity.y

velocity.x = velocity.x * 0.94
velocity.y = velocity.y * 0.94

parent:get_component_Table().table.velocity = velocity

local nearby_ents = scene:get_close_entities(parent, 2)
for i, other in ipairs(nearby_ents) do
    local oth_transform = other:get_component_Transform()
    oth_transform.rr = oth_transform.rr + velocity.x * 10 * pine_tick_dt()
end