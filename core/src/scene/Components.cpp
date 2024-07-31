#include "scene/Components.h"
#include "TexturePool.h"
#include "Serialize.h"
#include "scene/Entity.h"

#include <unordered_set>

#define PRINT_COMP_SIZE(COMP) \
    std::cout << #COMP": " << sizeof(COMP) << "\n"

void print_component_sizes() {
    PRINT_COMP_SIZE(component::Tag);
    PRINT_COMP_SIZE(component::Transform);
    PRINT_COMP_SIZE(component::CustomBehavior);
    PRINT_COMP_SIZE(component::Script);
    PRINT_COMP_SIZE(component::Table);
    PRINT_COMP_SIZE(component::Sprite);
    PRINT_COMP_SIZE(component::StateFlags);
}

namespace component {

Tag::Tag(std::string _name) noexcept : name(_name) {}

void Tag::deserialize(SceneNodeData &data) {
    CHECK_SERDE(data.variables.size() == 2 && data.variables.at(1).name == "name");
    VAR_FROM_NODE(name, data);
}

SceneNodeData Tag::serialize() const {
    return SceneNodeData { 
        .type=SceneNodeType::Component,
        .variables = { 
            COMP_TYPE(Tag),
            VAR_TO_NODE(name) 
        }
    };
}

glm::mat4 Transform::get_matrix() const {
    glm::mat4 output(1.0f);
    output = glm::translate(output, glm::vec3(x_interpolated, y_interpolated, 0.0f));
    output = glm::rotate(output, rr_interpolated, glm::vec3(0.0f, 0.0f, -1.0f));
    output = glm::scale(output, glm::vec3(sx_interpolated, sy_interpolated, 1.0f));
    return output;
}

void Transform::save_last_position() {
    x_0 = x;
    y_0 = y;
    sx_0 = sx;
    sy_0 = sy;
    rr_0 = rr;
}

void Transform::interpolate(float alpha) {
    x_interpolated = x * alpha + x_0 * (1.0f - alpha);
    y_interpolated = y * alpha + y_0 * (1.0f - alpha);
    sx_interpolated = sx * alpha + sx_0 * (1.0f - alpha);
    sy_interpolated = sy * alpha + sy_0 * (1.0f - alpha);
    rr_interpolated = rr * alpha + rr_0 * (1.0f - alpha);
}

void Transform::deserialize(SceneNodeData &data) {
    CHECK_SERDE(data.variables.size() == 11);

    VAR_FROM_NODE(x, data);
    VAR_FROM_NODE(y, data);
    VAR_FROM_NODE(x_0, data);
    VAR_FROM_NODE(y_0, data);
    VAR_FROM_NODE(sx, data);
    VAR_FROM_NODE(sy, data);
    VAR_FROM_NODE(sx_0, data);
    VAR_FROM_NODE(sy_0, data);
    VAR_FROM_NODE(rr, data);
    VAR_FROM_NODE(rr_0, data);
}

SceneNodeData Transform::serialize() const {
    return SceneNodeData { 
        .type=SceneNodeType::Component,
        .variables = {
            COMP_TYPE(Transform),
            VAR_TO_NODE(x),
            VAR_TO_NODE(y),
            VAR_TO_NODE(x_0),
            VAR_TO_NODE(y_0),
            VAR_TO_NODE(sx),
            VAR_TO_NODE(sy),
            VAR_TO_NODE(sx_0),
            VAR_TO_NODE(sy_0),
            VAR_TO_NODE(rr),
            VAR_TO_NODE(rr_0)
        }
    };
}

Script::Script(std::string str) noexcept {
    m_scripts.push_back(str);
}

Script::id_t Script::push_script(std::string str) { 
    m_scripts.push_back(str) ;
    return m_scripts.size() - 1;
}
// @Lua API
void Script::run(Script::id_t id) const { 
    auto &script = m_scripts.at(id);
    assert(!script.empty());

    auto &lua = LuaStateDispatcher::instance().get_lua();
    lua.set_function("pine_get_script_parent_entity", [this] { 
        assert(this->m_parent); return *this->m_parent; });
    ScriptEngine::run_script(lua, app_relative_path(script)); 
}
// @Lua API
void Script::run_all() const {
    auto &lua = LuaStateDispatcher::instance().get_lua();
    lua.set_function("pine_get_script_parent_entity", [this] { 
        assert(this->m_parent); return *this->m_parent; });

    for (auto &script : m_scripts) {
        ScriptEngine::run_script(lua, app_relative_path(script));
    }
}

void Script::deserialize(SceneNodeData &data) {
    CHECK_SERDE(data.variables.size() == 2);

    auto &scripts = m_scripts; // Remove the m_
    VAR_FROM_NODE(scripts, data);
}

SceneNodeData Script::serialize() const {
    auto &scripts = m_scripts; // Remove the m_
    return SceneNodeData { 
        .type=SceneNodeType::Component,
        .variables = { 
            COMP_TYPE(Script),
            VAR_TO_NODE(scripts)
        }
    };
}

CustomBehavior::CustomBehavior(std::string path) noexcept : on_update(path) {}
// @Lua API
CustomBehavior::~CustomBehavior() { 
    if (!on_remove.empty()) {
        auto &lua = LuaStateDispatcher::instance().get_lua();
        lua.set_function("pine_get_script_parent_entity", [this] { 
            assert(this->m_parent); return *this->m_parent; });

        ScriptEngine::run_script(lua, app_relative_path(on_remove));
    }
}

void CustomBehavior::set_on_update(std::string path) {
    on_update = path;
}

void CustomBehavior::set_on_remove(std::string path) {
    on_remove = path;
}

void CustomBehavior::deserialize(SceneNodeData &data) {
    CHECK_SERDE(data.variables.size() == 3);
    VAR_FROM_NODE(on_update, data);
    VAR_FROM_NODE(on_remove, data);
}

SceneNodeData CustomBehavior::serialize() const {
    return SceneNodeData {
        .type=SceneNodeType::Component,
        .variables = {
            COMP_TYPE(CustomBehavior),
            VAR_TO_NODE(on_update),
            VAR_TO_NODE(on_remove)
        }
    };
}

Sprite::Sprite(std::string path) noexcept : m_img(TexturePool::instance().push(app_relative_path(path))) {
    m_img->filter_nearest();
    m_save_string = std::make_shared<std::string>(path);
}

void Sprite::set_texture(std::string path) { 
    m_img = TexturePool::instance().push(app_relative_path(path));
    m_img->filter_nearest();
    m_save_string = std::make_shared<std::string>(path);
}

void Sprite::deserialize(SceneNodeData &data) {
    CHECK_SERDE(data.variables.size() == 3);

    std::string path;
    VAR_FROM_NODE(path, data);
    *this = Sprite(path);
    VAR_FROM_NODE(render_layer, data);
}

SceneNodeData Sprite::serialize() const {
    auto &path = *m_save_string;
    return SceneNodeData { 
        .type=SceneNodeType::Component,
        .variables = {
            COMP_TYPE(Sprite),
            VAR_TO_NODE(path),
            VAR_TO_NODE(render_layer)
        }
    };
}

Table::Table(sol::table _data) noexcept : table(_data) {}

void Table::deserialize(SceneNodeData &data) {
    CHECK_SERDE(data.variables.size() == 2);

    std::string table_string = data.variables.at(1).value;
    auto &lua = LuaStateDispatcher::instance().get_lua();
    auto result = lua.script("return " + table_string);
    assert(result.valid());
    table = result.get<sol::table>();
}

SceneNodeData Table::serialize() const {
    auto &lua = LuaStateDispatcher::instance().get_lua();

    lua.set_function("_pine_internals_get_table", [this] { return this->table; });
    auto result = ScriptEngine::run_script(lua, "../core/engine_lua/internal_table_serialize.lua");
    assert(result.valid());
    auto table_string = result.get<std::string_view>();

    return SceneNodeData { 
        .type=SceneNodeType::Component,
        .variables = {
            COMP_TYPE(Table),
            VAR_TO_NODE(table_string)
        }
    };
}

StateFlags::StateFlags(std::vector<std::string> flags) noexcept {
    set_flags(std::move(flags));
}

void StateFlags::set_flags(std::vector<std::string> flags) {
    for (auto &flag : flags) {
        if (m_has_flag(flag)) [[unlikely]] continue;
        m_flags.push_back(std::move(flag));
    }
}

bool StateFlags::has_flags(std::vector<std::string> flags) {
    for (const auto &flag : flags) {
        if (!m_has_flag(flag)) return false;
    }
    return true;
}

void StateFlags::remove_flags(std::vector<std::string> flags) {
    std::unordered_set<std::string> flags_set(flags.begin(), flags.end());
    m_flags.erase(std::remove_if(m_flags.begin(), m_flags.end(), 
        [&flags_set](const std::string& f) {
            return flags_set.find(f) != flags_set.end();
        }), 
    m_flags.end());
}


bool StateFlags::m_has_flag(const std::string &flag) {
    return std::any_of(m_flags.begin(), m_flags.end(), [&](const std::string &oth) {
        return oth == flag;
    });
}

void StateFlags::deserialize(SceneNodeData &data) {
    CHECK_SERDE(data.variables.size() == 2);
    auto &flags = m_flags;
    VAR_FROM_NODE(flags, data);
}

SceneNodeData StateFlags::serialize() const {
    auto &flags = m_flags;
    return SceneNodeData { 
        .type=SceneNodeType::Component,
        .variables = { 
            COMP_TYPE(StateFlags),
            VAR_TO_NODE(flags)
        }
    };
}

void Collider::type_box() {
    type = Type::AABB;
}

void Collider::type_circle() {
    type = Type::Circle;
}

const std::vector<CollisionData> &Collider::get_colliding_entities() const {
    return colliding_entities;
}

void Collider::deserialize(SceneNodeData &data) {
    uint8_t coll_type;
    VAR_FROM_NODE(coll_type, data);
    VAR_FROM_NODE(resolve_collisions, data);
    type = static_cast<Type>(coll_type);
}

SceneNodeData Collider::serialize() const {
    uint8_t coll_type = static_cast<uint8_t>(type);
    return SceneNodeData { 
        .type=SceneNodeType::Component,
        .variables = {
            COMP_TYPE(Collider),
            VAR_TO_NODE(coll_type),
            VAR_TO_NODE(resolve_collisions)
        }
    };
}

}