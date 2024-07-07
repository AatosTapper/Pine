#pragma once

#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "singleton.h"

namespace ec_system {

class TextureSetter {
public:
    TextureSetter() {}
    IMPL_SINGLETON_DISPATCHER(TextureSetter)
    IMPL_NO_COPY(TextureSetter)

    void update(Scene *scene) {
        return;
        /*
        for (auto &sprite : scene->components<component::Sprite>()) {
            auto &comp = sprite.component;
            if (comp.texture > 0) continue;
            //if (comp.temp_path == nullptr) [[likely]] continue;
            if (!comp.temp_path.empty()) [[likely]] {
                comp.texture = scene->get_textures()->push(comp.temp_path);
                comp.temp_path = nullptr;
                std::cout << "Texture assigned to a sprite\n"; 
            }        
        }
        */
    }
};

}