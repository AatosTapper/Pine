#include "scene/SceneManager.h"

void SceneManager::push(std::unique_ptr<Scene> scene) { 
    m_commands.push_back(ManagerCommand{ 
        .type = ManagerCommand::Type::Push,
        .scene = std::move(scene)
    });
}

void SceneManager::pop() {
    m_commands.push_back(ManagerCommand{ 
        .type = ManagerCommand::Type::Pop,
        .scene = nullptr
    });
}

uint32_t SceneManager::num_scenes() { 
    return m_scene_stack.size(); 
}

Scene *SceneManager::get_scene() const { 
    if (m_scene_stack.empty()) [[unlikely]] return nullptr;
    return m_scene_stack.top().get();
}

void SceneManager::set_camera(Camera *camera) {
    m_camera = camera; 
}

void SceneManager::m_push(std::unique_ptr<Scene> &&scene) {
    std::cout << "Pushed scene " << scene->name << "\n";
    scene->m_camera = m_camera;
    m_scene_stack.push(std::move(scene));
}

void SceneManager::m_pop() {
    if (m_scene_stack.empty()) [[unlikely]] return;
    std::cout << "Popped scene\n";
    m_scene_stack.pop();
}

void SceneManager::update() {
    for (auto &command : m_commands) {
        switch (command.type) {
        case ManagerCommand::Type::Push: {
            m_push(std::move(command.scene));
            break;
        } case ManagerCommand::Type::Pop: {
            m_pop();
            break;
        }
        }
    }
    m_commands.clear();
}

void SceneManager::try_update() {
    if (m_update_whenever) [[unlikely]] {
        update();
    }
}