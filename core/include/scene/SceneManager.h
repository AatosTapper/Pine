#pragma once

#include "pch.h"
#include <stack>

#include "Scene.h"

class SceneManager {
public:
    void push(std::unique_ptr<Scene> scene) { m_scene_stack.push(std::move(scene)); }
    void pop() {
        if (m_scene_stack.empty()) [[unlikely]] return;
        m_scene_stack.pop(); 
    }
    uint32_t num_scenes() { return m_scene_stack.size(); }

    Scene *get_scene() const { 
        if (m_scene_stack.empty()) [[unlikely]] return nullptr;
        return m_scene_stack.top().get(); 
    }
    
private:
    std::stack<std::unique_ptr<Scene>> m_scene_stack;
};