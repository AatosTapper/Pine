#pragma once

#include "pch.h"
#include <stack>

#include "Scene.h"

class SceneManager {
public:
    void push(std::unique_ptr<Scene> scene) { m_scene_stack.push(std::move(scene)); }
    void pop() { m_scene_stack.pop(); }

    Scene *get_scene() const { 
        if (m_scene_stack.empty()) [[unlikely]] return nullptr;
        return m_scene_stack.top().get(); 
    }
    
private:
    std::stack<std::unique_ptr<Scene>> m_scene_stack;
};