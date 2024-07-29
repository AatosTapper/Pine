#include "scene/SceneManager.h"

void SceneManager::push(std::unique_ptr<Scene> scene) { 
    m_commands.emplace_back(ManagerCommand{ 
        .type = ManagerCommand::Type::Push,
        .scene = std::move(scene)
    });
}

void SceneManager::pop() {
    m_commands.emplace_back(ManagerCommand{ 
        .type = ManagerCommand::Type::Pop,
        .scene = nullptr
    });
}

uint32_t SceneManager::num_scenes() const { 
    return m_scene_stack.size(); 
}

Scene *SceneManager::get_scene() const { 
    if (m_scene_stack.empty()) [[unlikely]] return nullptr;
    return m_scene_stack.back().get();
}

void SceneManager::m_push(std::unique_ptr<Scene> &&scene) {
    std::cout << "Pushed scene " << scene->name << "\n";
    m_scene_stack.push_back(std::move(scene));
    m_scene_stack.back()->m_camera->init(m_camera_data);
}

void SceneManager::m_pop() {
    if (m_scene_stack.empty()) [[unlikely]] return;
    std::cout << "Popped scene\n";
    m_scene_stack.pop_back();
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
    if (!m_restrict_updates) [[unlikely]] {
        update();
        m_scene_stack.back()->update();
    }
}

void SceneManager::set_camera_data(CameraData &&data) {
    m_camera_data = std::move(data);
}

void SceneManager::cam_aspect_ratio_callback(float aspect_ratio) {
    for (auto &scene : m_scene_stack) {
        scene->get_camera()->set_aspect_ratio(aspect_ratio);
    };
}