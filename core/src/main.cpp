#include "Application.h"
#include "ScriptEngine.h"
#include "scene/ecs.h"

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

    if (auto data = comp_storage.get_component<Component2>(ent1)) {
        auto &comp = data->get();
        std::cout << comp.bro << std::endl;
    }

    sol::state lua = ScriptEngine::create_lua_state();
    Application app(lua);
    app.entry();

    return 0;
}