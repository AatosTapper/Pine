local Cone = require("core.Cone.cone")

-- It's a bit weird that the spawner entity's on_update
-- handles the camera controls but here's just a small example.

-- In real world, you would want to have another entity like: camera_controller
-- with a CustomBehavior on_update script that handles it


if pine_get_input(Cone.Key._W)  then
    Cone.Event.create("SomeEvent", {
        test = 2
    })
end

-- Getting the relevant variables
local scene = pine_get_scene()
local parent = pine_get_script_parent_entity()
local camera = scene:get_camera()

-- simple controller using pine_get_input(keycode)
local speed = 18 * pine_tick_dt()

if pine_get_input(Cone.Key._W) then
    camera:up(speed)
end
if pine_get_input(Cone.Key._S) then
    camera:down(speed)
end
if pine_get_input(Cone.Key._A) then
    camera:left(speed)
end
if pine_get_input(Cone.Key._D) then
    camera:right(speed)
end
if pine_get_input(Cone.Key._MOUSE_BUTTON_LEFT) then
    camera:left(speed)
end
if pine_get_input(Cone.Key._MOUSE_BUTTON_RIGHT) then
    camera:right(speed)
end