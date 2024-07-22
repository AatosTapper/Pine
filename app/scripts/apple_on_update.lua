local entity = pine_get_script_parent_entity()

local tbl = entity:get_component_Table().table
local transform = entity:get_component_Transform()

transform.rr = transform.rr + tbl.rotation_speed * pine_tick_dt()
-- lua doesn't have +=   wtf :((