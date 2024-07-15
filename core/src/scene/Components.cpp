#include "scene/Components.h"
#include "TexturePool.h"
#include "Serialize.h"
#include "scene/Entity.h"

namespace component {

Tag::Tag(std::string _name) noexcept : name(_name) {}

void Tag::deserialize(NodeData &data) {
    CHECK_SERDE(data.variables.size() == 1 && data.variables.at(0).name == "name");
    VAR_FROM_NODE(name, data);
}

NodeData Tag::serialize() const {
    return NodeData { 
        .type=NodeType::Component,
        .variables = { 
            COMP_TYPE(Tag),
            VAR_TO_NODE(name) 
        }
    };
}

glm::mat4 Transform::get_matrix() const
{
    /// @todo clamp rr and check that scales aren't 0
    glm::mat4 output(1.0f);
    output = glm::translate(output, glm::vec3(x, y, 0.0f));
    output = glm::rotate(output, rr, glm::vec3(0.0f, 0.0f, -1.0f));
    output = glm::scale(output, glm::vec3(sx, sy, 1.0f));
    return output;
}

void Transform::deserialize(NodeData &data) {
    CHECK_SERDE(data.variables.size() == 5);

    VAR_FROM_NODE(x, data);
    VAR_FROM_NODE(y, data);
    VAR_FROM_NODE(sx, data);
    VAR_FROM_NODE(sy, data);
    VAR_FROM_NODE(rr, data);
}

NodeData Transform::serialize() const {
    return NodeData { 
        .type=NodeType::Component,
        .variables = {
            COMP_TYPE(Transform),
            VAR_TO_NODE(x),
            VAR_TO_NODE(y),
            VAR_TO_NODE(sx),
            VAR_TO_NODE(sy),
            VAR_TO_NODE(rr)
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

    sol::state &lua = LuaStateDispatcher::instance().get_lua();
    lua.set_function("pine_get_script_parent_entity", [this] { 
        assert(this->m_parent); return *this->m_parent; });
    ScriptEngine::run_script(lua, app_relative_path(script)); 
}
// @Lua API
void Script::run_all() const {
    sol::state &lua = LuaStateDispatcher::instance().get_lua();
    lua.set_function("pine_get_script_parent_entity", [this] { 
        assert(this->m_parent); return *this->m_parent; });

    for (auto &script : m_scripts) {
        ScriptEngine::run_script(lua, app_relative_path(script));
    }
}

void Script::deserialize(NodeData &data) {
    CHECK_SERDE(data.variables.size() == 1);

    auto &scripts = m_scripts; // Remove the m_
    VAR_FROM_NODE(scripts, data);
}

NodeData Script::serialize() const {
    auto &scripts = m_scripts; // Remove the m_
    return NodeData { 
        .type=NodeType::Component,
        .variables = { 
            COMP_TYPE(Script),
            VAR_TO_NODE(scripts)
        }
    };
}

CustomBehaviour::CustomBehaviour(std::string path) noexcept : m_on_update(path) {}
// @Lua API
CustomBehaviour::~CustomBehaviour() { 
    if (!m_on_remove.empty()) {
        auto &lua = LuaStateDispatcher::instance().get_lua();
        lua.set_function("pine_get_script_parent_entity", [this] { 
            assert(this->m_parent); return *this->m_parent; });

        ScriptEngine::run_script(lua, app_relative_path(m_on_remove));
    }
}

void CustomBehaviour::set_on_update(std::string path) {
    m_on_update = path;
}

void CustomBehaviour::set_on_remove(std::string path) {
    m_on_remove = path;
}
// @Lua API
void CustomBehaviour::call_on_update() const { 
    if (!m_on_update.empty()) [[likely]] {
        auto &lua = LuaStateDispatcher::instance().get_lua();
        lua.set_function("pine_get_script_parent_entity", [this] { 
            assert(this->m_parent); return *this->m_parent; });

        ScriptEngine::run_script(lua, app_relative_path(m_on_update));
    }
}

void CustomBehaviour::deserialize(NodeData &data) {
    CHECK_SERDE(data.variables.size() == 1);

    std::string on_update;
    std::string on_remove;
    VAR_FROM_NODE(on_update, data);
    VAR_FROM_NODE(on_remove, data);
    m_on_update = on_update;
    m_on_remove = on_remove;
}

NodeData CustomBehaviour::serialize() const {
    std::string on_update = m_on_update;
    std::string on_remove = m_on_remove;
    return NodeData {
        .type=NodeType::Component,
        .variables = {
            COMP_TYPE(CustomBehaviour),
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

void Sprite::deserialize(NodeData &data) {
    CHECK_SERDE(data.variables.size() == 1);

    std::string path;
    VAR_FROM_NODE(path, data);
    *this = Sprite(path);
}

NodeData Sprite::serialize() const {
    auto &path = *m_save_string;
    return NodeData { 
        .type=NodeType::Component,
        .variables = {
            COMP_TYPE(Sprite),
            VAR_TO_NODE(path)
        }
    };
}

Table::Table(sol::table _data) noexcept : table(_data) {}

void Table::deserialize(NodeData &data) {
    CHECK_SERDE(data.variables.size() == 1);

    std::string table_string = data.variables[0].value;
    auto &lua = LuaStateDispatcher::instance().get_lua();
    lua.set_function("_pine_internals_get_table_string", [&] { return table_string; });

    auto result = lua.script(R"(
        Cone = require("core.Cone.cone")
        return Cone.string_to_table(_pine_internals_get_table_string())
    )");
    assert(result.valid());
    table = result.get<sol::table>();
}

NodeData Table::serialize() const {
    auto &lua = LuaStateDispatcher::instance().get_lua();

    lua.set_function("_pine_internals_get_table", [this] { return this->table; });
    auto result = lua.script(R"(
        Cone = require("core.Cone.cone")
        return Cone.to_string(_pine_internals_get_table())
    )");
    assert(result.valid());
    std::string_view table_string = result.get<std::string_view>();

    return NodeData { 
        .type=NodeType::Component,
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

bool StateFlags::m_has_flag(const std::string &flag) {
    return std::any_of(m_flags.begin(), m_flags.end(), [&](const std::string &oth) {
        return oth == flag;
    });
}

void StateFlags::deserialize(NodeData &data) {
    CHECK_SERDE(data.variables.size() == 1);
    auto &flags = m_flags;
    VAR_FROM_NODE(flags, data);
}

NodeData StateFlags::serialize() const {
    auto &flags = m_flags;
    return NodeData { 
        .type=NodeType::Component,
        .variables = { 
            COMP_TYPE(StateFlags),
            VAR_TO_NODE(flags)
        }
    };
}

}