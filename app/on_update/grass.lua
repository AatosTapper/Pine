local scene = pine_get_scene()

-- local gives about 30% performance boost

-- taking references of the global functions into local variables
local ipairs = ipairs
local sin = math.sin
local pow = math.pow

local normalized_dt = pine_normalized_dt()
local current_time = pine_current_time()

local seed = current_time * 100
local acc = pow(0.2, 1 / normalized_dt)

local entities = scene:get_entities()

for i, ent in ipairs(entities) do
    local state_flags = ent:get_component_StateFlags()
    if state_flags and state_flags:has_flags({ "grass_interactable" }) then
        local transform = ent:get_component_Transform()
        local sway = (sin(seed * 0.017 + transform.y) * 0.024)
                   + (sin(seed * 0.2 + transform.x) * 0.0014)
        sway = sway * normalized_dt

        transform.rr = transform.rr + transform.rr * (-acc) / normalized_dt + sway
    end
end