#include "Application.h"
#include "ScriptEngine.h"
#include "scene/ecs.h"
#include "scene/Scene.h"
#include "scene/Entity.h"
#include "scene/Components.h"

struct Component1 {
    int bro = 0;
};

struct Component2 {
    Component2() = delete;
    Component2(int val) : bro(val) {}
    int bro = 0;
};

int main() {
    ecs::ComponentStorage comp_storage;

    auto ent1 = comp_storage.add_entity();
    auto ent2 = comp_storage.add_entity();

    comp_storage.add_component<Component1>(ent1);
    comp_storage.add_component<Component2>(ent1, -21);
    comp_storage.add_component<Component2>(ent2, 10);

    // get component
    if (auto data = comp_storage.get_component<Component2>(ent1)) {
        auto &comp = data->get();
        //std::cout << comp.bro << std::endl;
    }

    // iterate single components
    for (auto data : comp_storage.components<Component2>()) {
        auto &comp = *data;
        //std::cout << comp.bro << std::endl;
    }

    // iterate multiple components
    // this only goes though the entities with the iterator type component
    // i think adding some sort of tag component for this is ideal
    for (auto it = comp_storage.begin<Component2>(); it != comp_storage.end<Component2>(); it++) {
        auto [c2, c1, f] = comp_storage.get_multiple_components<Component2, Component1, float>(it);
        if (auto val = c2) {
            auto &comp = val->get();
            std::cout << comp.bro << std::endl;
        }
        if (auto val = c1) {
            auto &comp = val->get();
            std::cout << comp.bro << std::endl;
        }
    }

    Scene scene;
    auto entity1 = scene.add_entity("testi");
    auto entity2 = scene.add_entity();

    entity1.add_component<Component2>(1000);
    entity1.add_component<Component1>();
    entity1.add_component<float>();
    entity1.remove();
    entity1 = scene.add_entity("kakkeli");
    if (auto bruh = entity1.get_component<component::Tag>()) {
        std::cout << bruh->get().name << std::endl;
    }

    sol::state lua = ScriptEngine::create_lua_state();
    Application app(lua);
    app.entry();

    return 0;
}