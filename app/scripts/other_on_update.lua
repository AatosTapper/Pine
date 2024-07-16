local entity = pine_get_script_parent_entity()
if entity == nil then
    print("entity was nil")
    return
end

if entity:has_component_Table() then
    local tbl = entity:get_component_Table()
    local transform = entity:get_component_Transform()
    transform.rr = transform.rr + tbl.table.rotation_coeff * pine_fixed_update_dt()
end
