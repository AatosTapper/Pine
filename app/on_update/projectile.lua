local parent = pine_get_script_parent_entity()
local scene = pine_get_scene()
local transform = parent:get_component_Transform()
local data = parent:get_component_Table()

if data.table.lifetime <= 0 then
    parent:remove()
    return
end
data.table.lifetime = data.table.lifetime - 1 * pine_normalized_dt()

local speed = 0.9 * pine_normalized_dt()

transform.x = transform.x + data.table.direction.x * speed
transform.y = transform.y + data.table.direction.y * speed


local nearby_ents = scene:get_close_entities(parent, 1)
for i, v in ipairs(nearby_ents) do
    if v:has_component_StateFlags() then
        if v:get_component_StateFlags():has_flags({ "grass_interactable" }) then
            v:remove()
        end
    end
end