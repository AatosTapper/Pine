local scene = pine_get_scene()

for i, ent in ipairs(scene:get_entities()) do
    if ent:has_component_StateFlags() and ent:get_component_StateFlags():has_flags({ "grass_interactable" }) then
        local transform = ent:get_component_Transform()

        local seed = pine_current_time() * 100
        local sway = (math.sin(seed * 0.017 + transform.y) * 0.024)
                + (math.sin(seed * 0.2 + transform.x) * 0.002)

        sway = sway * pine_normalized_dt()

        local acc = math.pow(0.2, 1 / pine_normalized_dt())
        transform.rr = transform.rr + transform.rr * (-acc) / pine_normalized_dt() + sway
    end
end