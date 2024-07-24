local Cone = require("core.Cone.cone")
-- This script spawns an apple with some stuff

local scene = pine_get_scene()

-- Creating an entity and giving it a random number as name
local name = tostring(math.random(0, 100))
local ent = scene:add_entity(name)
-- The name is actually stored in the Tag component
-- Every entity has a Tag component and a Transform component by default

-- Adding a sprite to the entity
local sprite = ent:add_component_Sprite()
sprite:set_texture("res/textures/apple.png")

-- Giving the entity a random position, scale and rotation
local transform = ent:get_component_Transform()
-- position
transform.x = math.random() * 64 - 32
transform.y = math.random() * 32 - 16
-- scale (width and height)
transform.sx = (math.random() + 0.25) * 2
transform.sy = (math.random() + 0.25) * 2
-- rotation in radians
transform.rr = math.random() * 8

-- Adding a Table component to hold some variables
local table_component = ent:add_component_Table()
table_component.table = {
    -- this is used to give the apples a random rotation speed
    rotation_speed = (math.random() - 0.5) * 5
}

-- Example on how to add flags
local flags = ent:add_component_StateFlags({ "flag1", "whatever_flag2" })

-- Adding a CustomBehavior component to define the frame by frame logic
-- Here it just rotates the apple based on the rotation_speed
local cb = ent:add_component_CustomBehavior("scripts/apple_on_update.lua")

Cone.Event.listener("SomeEvent", function(data)
    --print("bruh")
    --
end)