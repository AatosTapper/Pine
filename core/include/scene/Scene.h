#pragma once

#include "ecs.h"

class Entity;
class Scene {
    friend class Entity;
public:
    ecs::ComponentStorage &get_storage() {
        return m_storage;
    }
    
    Entity add_entity(std::string name = "");

private:
    ecs::ComponentStorage m_storage;
};
