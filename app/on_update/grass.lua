local parent = pine_get_script_parent_entity()

local transform = parent:get_component_Transform()

local seed = pine_frame_index()
local sway = (math.sin(seed * 0.017 + transform.y) * 0.024)
           + (math.sin(seed * 0.2 + transform.x) * 0.002)

transform.rr = transform.rr + transform.rr * (-0.2) + sway
