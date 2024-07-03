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
    Scene scene;
    auto entity1 = scene.add_entity("testi");
    auto entity2 = scene.add_entity("3");
    auto entity3 = scene.add_entity("asad");
    entity1.add_component<Component1>();
    entity2.add_component<Component1>();
    entity1.add_component<Component2>(69);
    entity2.add_component<Component2>(420);

    for (auto it = scene.begin<component::Tag>(); it != scene.end<component::Tag>(); it++) {
        auto result = scene.get_multiple_components<Component1, Component2>(it);
        std::cout << it->component.name << " loop\n";
        auto &[a, b] = result;
        if (auto val = b) {
            std::cout << val->get().bro << std::endl;
        } else {
            std::cout << "no component" << std::endl;
        }
    }

    sol::state lua = ScriptEngine::create_lua_state();
    Application app(lua);
    app.entry();

    return 0;
}