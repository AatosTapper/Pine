local parent = pine_get_script_parent_entity()
local scene = pine_get_scene()
local transform = parent:get_component_Transform()
local data = parent:get_component_Table()

if data.table.lifetime <= 0 then
    parent:remove()
    return
end

local normalized_dt = pine_normalized_dt()

data.table.lifetime = data.table.lifetime - 1 * normalized_dt


local speed = 0.9 * normalized_dt

transform.x = transform.x + data.table.direction.x * speed
transform.y = transform.y + data.table.direction.y * speed


local nearby_ents = scene:get_close_entities(parent, 1)
for i, v in ipairs(nearby_ents) do
    local flags = v:get_component_StateFlags()
    if flags and flags:has_flags({ "grass_interactable" }) then
        v:get_component_Table().table.cut = true
        v:get_component_Table().table.timer = 0
    end
end