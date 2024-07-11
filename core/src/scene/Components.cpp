#include "scene/Components.h"
#include "TexturePool.h"
#include "Serialize.h"

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
            VAR_TO_NODE(name) 
        }
    };
}

glm::mat4 Transform::get_matrix() const
{
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
void Script::run(sol::state &lua, Script::id_t id) const { 
    auto &script = m_scripts.at(id);
    assert(!script.empty()); 
    ScriptEngine::run_script(lua, app_relative_path(script)); 
}

void Script::run_all(sol::state &lua) const {
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
            VAR_TO_NODE(scripts)
        }
    };
}

CustomBehaviour::CustomBehaviour(sol::function f) noexcept : m_on_update(f) {}

CustomBehaviour::~CustomBehaviour() { 
    if (m_on_remove) m_on_remove.call(); 
}

void CustomBehaviour::set_on_update(sol::function func) {
    assert(func && "not a valid callback");
    m_on_update = std::move(func);
}

void CustomBehaviour::set_on_remove(sol::function func) {
    assert(func && "not a valid callback");
    m_on_remove = std::move(func);
}

void CustomBehaviour::call_on_update() const { 
    if (m_on_update) {
        try {
            m_on_update.call();
        } catch (const sol::error& e) {
            std::cerr << "Error in CustomBehaviour::on_update lua function: " << e.what() << std::endl;
        }
    }
}

Sprite::Sprite(std::string path) noexcept : m_img(TexturePool::instance().push(app_relative_path(path))) {
    m_img->filter_nearest();
}

void Sprite::set_texture(std::string path) { 
    m_img = TexturePool::instance().push(app_relative_path(path));
    m_img->filter_nearest();
}

}