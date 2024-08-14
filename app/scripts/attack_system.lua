local attack_system = {}

function create_attack_base()
    
    return{
        dir = vec2.normalize(pine_get_mouse_pos() - vec2.new(pine_window_width() / 2, pine_window_height() / 2)),
        lifetime = 0,
        cooldown = 0,
        
        execute = function(origin, direction, flags)
        print("this function doesn't exist")
        end 
    }
    
end

function attack_system.create_attack_sword()
 local attack = create_attack_base()
 attack.cooldown = 1
 attack.lifetime = 0.5
 attack.execute = function (origin, direction, flags) 
     local scene = pine_get_scene()
     local ent = scene:add_entity("slash")
 end
    return 
    {

    }
end

function attack_system.create_attack_apple()
    
    local attack = create_attack_base()
    attack.cooldown = 1
    attack.lifetime = 0.5
    attack.execute = function (origin, dir, flags) 
        
        local scene = pine_get_scene()
        local ent = scene:add_entity("projectile")
        ent:get_component_Transform().x = origin.x
        ent:get_component_Transform().y = origin.y
    
        ent:add_component_Sprite("res/textures/apple.png")
        ent:add_component_Table({
            direction = { x = dir.x, y = -dir.y },
            lifetime = 20
        })
        ent:add_component_CustomBehavior("on_update/projectile.lua")


    end
       return attack
   end

   return attack_system

   