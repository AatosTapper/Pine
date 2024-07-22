-- This script spawns an apple with some stuff

local scene = pine_get_scene()
if scene == nil then
    print("Scene was nil in script")
    return
end

local name = tostring(math.random(0, 100))
local ent = scene:add_entity(name)

local sprite = ent:add_component_Sprite()
sprite:set_texture("res/textures/apple.png")

local transform = ent:get_component_Transform()
transform:set_pos(math.random() * 64 - 32, math.random() * 32 - 16)
transform:set_scale((math.random() + 0.25) * 2, (math.random() + 0.25) * 2)
transform.rr = math.random() * 8

local table_component = ent:add_component_Table()
table_component.table = {
    rotation_coeff = (math.random() - 0.5) * 5,
    timer = math.random(10, 1000)
}

local flags = ent:add_component_StateFlags({ "flag1" })

local cb = ent:add_component_CustomBehavior("scripts/apple_on_update.lua")

-- If called, this excludes the entity from savefiles
--ent:disable_serialization()