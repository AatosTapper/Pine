local scene = pine_get_scene()
if scene == nil then
    print("Scene was nil in script")
    return
end

local val = tostring(math.random(0, 100))
local ent = scene:add_entity(val)

local sprite = ent:add_component_Sprite()
sprite:set_texture("res/textures/apple.png")

local transform = ent:get_component_Transform()
transform:set_pos(math.random() * 64 - 32, math.random() * 32 - 16)
transform:set_scale((math.random() + 0.25) * 2, (math.random() + 0.25) * 2)
transform.rr = math.random() * 8

local table = ent:add_component_Table({
    rotation_coeff = (math.random() - 0.5) * 5
})

local flags = ent:add_component_StateFlags({ "brotha" })

ent:disable_serialization()