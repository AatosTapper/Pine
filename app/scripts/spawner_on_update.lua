local Cone = require("core.Cone.cone")

local parent = pine_get_script_parent_entity()
if parent == nil then
    print("parent was nil")
    return
end



--print("Ft: " .. Cone.Math.round_to_decimal(pine_frame_time() * 10^3, 2) .. "ms")

local scene = pine_get_scene()
if scene == nil then
    print("Scene was nil in script")
    return
end

local camera = scene:get_camera()

-- simple controller using pine_get_input(keycode)
local dt = pine_tick_dt()
local speed = 18 * dt

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

if pine_get_input(Cone.Key._SPACE) then
    local loading_scene = pine_set_temp_scene(pine_Scene:new("loading_scene"))
    local loading_ent = loading_scene:add_entity("loader")
    local comp = loading_ent:add_component_CustomBehavior()
    comp:set_on_update("scripts/loading_on_update.lua")
    loading_ent:add_component_Sprite("res/textures/rock.png")
end

--local mouse_pos = pine_get_mouse_pos();
--local pos = vec2.from(camera:get_pos())
--camera:set_pos(vec2.from(pos.x + ((math.random() - 0.5) * 0.2), pos.y + ((math.random() - 0.5) * 0.2)))