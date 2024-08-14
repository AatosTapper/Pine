local attack_system = {}

function create_attack_base()
    return {
        cooldown_timer = 0,
        cooldown_threshold = 10,
        execute = function(self, origin, direction, flags, attack_data)
            self.cooldown_timer = math.min(self.cooldown_timer, self.cooldown_threshold)
            if self.cooldown_timer >= self.cooldown_threshold then
                self.implementation(origin, direction, flags)
                self.cooldown_timer = self.cooldown_timer - self.cooldown_threshold
            end
        end,
        implementation = function(origin, direction, flags, attack_data)
            assert("Bruh implement the implementation")
        end
    }
end

function attack_system.create_attack_sword()
    local attack = create_attack_base()
    attack.cooldown_timer = 0
    attack.cooldown_threshold = 20
    attack.implementation = function (origin, direction, flags, attack_data)
        local scene = pine_get_scene()
        local ent = scene:add_entity("slash")
    end
    return attack
end

function attack_system.create_attack_apple()
    local attack = create_attack_base()
    attack.cooldown_timer = 0
    attack.cooldown_threshold = 20
    attack.implementation = function (origin, dir, flags, attack_data)
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

   