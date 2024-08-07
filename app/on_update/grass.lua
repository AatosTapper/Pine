local scene = pine_get_scene()

local growning_speed = 0.3

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
        
        if ent:get_component_Table().table.cut == true then
            
           
            
            ent:get_component_Table().table.timer = ent:get_component_Table().table.timer + growning_speed * math.random() / pine_normalized_dt()
            
            

--checking the current "life" of the grass

            if ent:get_component_Table().table.timer <= 0 then
                ent:get_component_Sprite():set_texture("res/textures/GrassTextures/Grass1.png")
            elseif ent:get_component_Table().table.timer <= 20 then
                ent:get_component_Sprite():set_texture("res/textures/GrassTextures/Grass2.png")
            elseif ent:get_component_Table().table.timer <= 60 then
                ent:get_component_Sprite():set_texture("res/textures/GrassTextures/Grass3.png")
            elseif ent:get_component_Table().table.timer <= 80 then
                ent:get_component_Sprite():set_texture("res/textures/GrassTextures/Grass4.png")
            elseif ent:get_component_Table().table.timer <= 100 then
                ent:get_component_Sprite():set_texture("res/textures/GrassTextures/Grass5.png")
                ent:get_component_Table().table.cut = false
                ent:get_component_Table().table.timer = 0
            end

           
                

           

            
        end
    end
end