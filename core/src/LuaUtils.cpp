#include "LuaUtils.h"

#include "pch.h"
#include "config.h"

static void set_glm(sol::state &lua) {

    /// VEC 2
    auto vec2_add = sol::overload(
        [](const glm::vec2 &a, const glm::vec2 &b) -> glm::vec2 { return a + b; }
    );
    auto vec2_sub = sol::overload(
        [](const glm::vec2 &a, const glm::vec2 &b) -> glm::vec2 { return a - b; }
    );
    auto vec2_mul = sol::overload(
        [](const glm::vec2 &v, float scalar) -> glm::vec2 { return v * scalar; },
        [](float scalar, const glm::vec2 &v) -> glm::vec2 { return scalar * v; }
    );
    auto vec2_div = sol::overload(
        [](const glm::vec2 &v, float scalar) -> glm::vec2 { return v / scalar; }
    );
    auto vec_2_from = sol::overload(
        [](const glm::vec3 &oth) -> glm::vec2 { return glm::vec2(oth.x, oth.y); },
        [](float a, float b) -> glm::vec2 { return glm::vec2(a, b); }
    );

    lua.new_usertype<glm::vec2>("vec2",
        sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
        // Members
        "x", &glm::vec2::x,
        "y", &glm::vec2::y,
        // Methods
        "set", [](glm::vec2 &self, float a, float b) { self.x = a; self.y = b; },
        "from", vec_2_from,
        "length", [](const glm::vec2 &v) { return glm::length(v); },
        "normalize", [](const glm::vec2 &v) { return glm::normalize(v); },
        "dot", [](const glm::vec2 &a, const glm::vec2 &b) { return glm::dot(a, b); },
        // Meta functions
        sol::meta_function::addition, vec2_add,
        sol::meta_function::subtraction, vec2_sub,
        sol::meta_function::multiplication, vec2_mul,
        sol::meta_function::division, vec2_div
    );

    /// VEC 3
    auto vec3_add = sol::overload(
        [](const glm::vec3 &a, const glm::vec3 &b) -> glm::vec3 { return a + b; }
    );
    auto vec3_sub = sol::overload(
        [](const glm::vec3 &a, const glm::vec3 &b) -> glm::vec3 { return a - b; }
    );
    auto vec3_mul = sol::overload(
        [](const glm::vec3 &v, float scalar) -> glm::vec3 { return v * scalar; },
        [](float scalar, const glm::vec3 &v) -> glm::vec3 { return scalar * v; }
    );
    auto vec3_div = sol::overload(
        [](const glm::vec3 &v, float scalar) -> glm::vec3 { return v / scalar; }
    );
    auto vec_3_from = sol::overload(
        [](const glm::vec2 &oth) -> glm::vec3 { return glm::vec3(oth.x, oth.y, 0.0f); }
    );

    lua.new_usertype<glm::vec3>("vec3",
        sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
        // Members
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z,
        // Methods
        "set", [](glm::vec3 &self, float a, float b, float c) { self.x = a; self.y = b, self.z = c; },
        "from", vec_3_from,
        "length", [](const glm::vec3 &v) { return glm::length(v); },
        "normalize", [](const glm::vec3 &v) { return glm::normalize(v); },
        "dot", [](const glm::vec3 &a, const glm::vec3 &b) { return glm::dot(a, b); },
        "cross", [](const glm::vec3 &a, const glm::vec3 &b) { return glm::cross(a, b); },
        // Meta functions
        sol::meta_function::addition, vec3_add,
        sol::meta_function::subtraction, vec3_sub,
        sol::meta_function::multiplication, vec3_mul,
        sol::meta_function::division, vec3_div
    );
}

void set_lua_utils(sol::state &lua) {
    set_glm(lua);
}