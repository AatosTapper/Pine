local scene = pine_get_scene()
local parent = pine_get_script_parent_entity()

local transform = parent:get_component_Transform()
local velocity = parent:get_component_Table().table.velocity

local speed = 0.019
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

transform.rr = transform.rr - transform.rr * 0.3
transform.rr = transform.rr + acceleration.x * 2

parent:get_component_Table().table.velocity = velocity

parent:get_component_Sprite():set_render_layer(-59.5 - transform.y)

local max_radius = 2

local nearby_ents = scene:get_close_entities(parent, max_radius)
for i, other in ipairs(nearby_ents) do
    if other:has_component_StateFlags() then
        if other:get_component_StateFlags():has_flags({ "grass_interactable" }) then
            local oth_transform = other:get_component_Transform()
            local distance = vec2.length(vec2.new(oth_transform.x, oth_transform.y) - vec2.new(transform.x, transform.y))
            oth_transform.rr = oth_transform.rr + velocity.x / math.max(distance * 2, 1.0) * 3
            oth_transform.rr = math.min(math.max(oth_transform.rr, -math.pi / 2), math.pi / 2)
        end
    end
end