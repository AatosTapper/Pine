local scene = pine_get_scene()
local parent = pine_get_script_parent_entity()

local transform = parent:get_component_Transform()
local data = parent:get_component_Table()
local velocity = data.table.velocity

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

local acceleration = frame_movement:normalize() * speed / pine_normalized_dt()
velocity.x = velocity.x + acceleration.x
velocity.y = velocity.y + acceleration.y

transform.x = transform.x + velocity.x / pine_normalized_dt()
transform.y = transform.y + velocity.y / pine_normalized_dt()

velocity.x = velocity.x * math.pow(0.94, 1 / pine_normalized_dt())
velocity.y = velocity.y * math.pow(0.94, 1 / pine_normalized_dt())

transform.rr = transform.rr - transform.rr * 0.3
transform.rr = transform.rr + acceleration.x * 3

data.table.velocity = velocity

parent:get_component_Sprite():set_render_layer(-39.5 - transform.y)


local max_radius = 2

--- Making the close entities move
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

--- Shooting the apple
function spawn_projectile_from_player()
    local dir = vec2.normalize(pine_get_mouse_pos() - vec2.new(pine_window_width() / 2, pine_window_height() / 2))
    local ent = scene:add_entity("projectile")
    ent:get_component_Transform().x = transform.x
    ent:get_component_Transform().y = transform.y

    ent:add_component_Sprite("res/textures/apple.png")
    ent:add_component_Table({
        direction = { x = dir.x, y = -dir.y },
        lifetime = 20
    })
    ent:add_component_CustomBehavior("on_update/projectile.lua")
end
