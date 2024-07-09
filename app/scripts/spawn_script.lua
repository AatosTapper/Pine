local scene = pine_get_scene()
if scene == nil then
    print("Scene was nil in script")
    return
end

local val = tostring(math.random(0, 100))
local ent = scene:add_entity(val)
if ent == nil then
    print("Failed to add entity")
    return
end

ent:add_component_Sprite()
local sprite = ent:add_component_Sprite("../app/res/textures/apple.png")
if sprite == nil then
    print("Failed to get Sprite component")
    return
end
--ent:get_component_Sprite():set_texture("../app/res/textures/apple.png")

local transform = ent:get_component_Transform()
if transform == nil then
    print("Failed to get Transform component")
    return
end
transform:set_pos(math.random() * 18 - 9, math.random() * 12 - 6)
transform:set_scale(math.random() + 0.5, math.random() + 0.5)
