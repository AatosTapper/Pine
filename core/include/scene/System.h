#pragma once

#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"
#include "singleton.h"

using SystemFunction = std::function<void(Scene *)>;
class System {
public:
    virtual ~System() {};

    virtual void update(Scene *scene) = 0;
};

class CustomBehaviourSystem : public System {
public:
    IMPL_VIRTUAL_SINGLETON_DISPATCHER(CustomBehaviourSystem, System)
    virtual ~CustomBehaviourSystem() override {}
    virtual void update(Scene *scene) override {
        for (auto ent : scene->view<component::CustomBehaviour>()) {
            Entity entity{ent, scene};
            auto &customBehaviour = entity.get_component<component::CustomBehaviour>();
            customBehaviour.call_on_update();
        }
    };
};