#include "scene/SceneManager.h"

void SceneManager::push(std::unique_ptr<Scene> scene) { 
    auto &temp = scene;
    temp->m_camera = m_camera;
    m_scene_stack.push(std::move(scene));
}

void SceneManager::pop() {
    if (m_scene_stack.empty()) [[unlikely]] return;
    m_scene_stack.pop();
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