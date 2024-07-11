#include "scene/Components.h"
#include "TexturePool.h"

namespace component {

glm::mat4 Transform::get_matrix() const
{
    glm::mat4 output(1.0f);
    output = glm::translate(output, glm::vec3(x, y, 0.0f));
    //output = glm::rotate(output, rotation_rad, glm::vec3(0.0f, 0.0f, -1.0f));
    output = glm::scale(output, glm::vec3(sx, sy, 1.0f));
    return output;
}

Script::Script(std::string str) noexcept {
    m_scripts.push_back(app_relative_path(str));
}

Script::id_t Script::push_script(std::string str) { 
    m_scripts.push_back(app_relative_path(str));
    return m_scripts.size() - 1;
}
void Script::run(sol::state &lua, Script::id_t id) { 
    auto &script = m_scripts.at(id);
    assert(!script.empty()); 
    ScriptEngine::run_script(lua, script); 
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