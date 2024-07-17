local entity = pine_get_script_parent_entity()
local transform = entity:get_component_Transform()

transform.rr = transform.rr + 10 * pine_tick_dt()

if pine_get_input(Cone.Key._Q) then
    pine_remove_temp_scene()
end