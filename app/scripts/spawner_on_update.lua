local parent = pine_get_script_parent_entity()
if parent == nil then
    print("parent was nil")
    return
end


local scene = pine_get_scene()
if scene == nil then
    print("Scene was nil in script")
    return
end

local camera = scene:get_camera()

-- simple controller using pine_get_input(keycode)
local dt = pine_fixed_update_dt()
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

-- Rotating the entities
local entitites = scene:get_entities()
for k, v in ipairs(entitites) do
    local transform = v:get_component_Transform()

    if v:has_component_Table() then
        local t = v:get_component_Table()
        transform.rr = transform.rr + t.table.rotation_coeff * dt
    end
    if v:has_component_StateFlags() then
        local comp = v:get_component_StateFlags()
        -- do something with the flags maybe
    end
end

--local mouse_pos = pine_get_mouse_pos();
--local pos = vec2.from(camera:get_pos())
--camera:set_pos(vec2.from(pos.x + ((math.random() - 0.5) * 0.2), pos.y + ((math.random() - 0.5) * 0.2)))