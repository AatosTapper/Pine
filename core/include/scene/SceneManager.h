#pragma once

#include "pch.h"
#include "Scene.h"
#include "rendering/Camera.h"

#include <stack>

class SceneManager {
public:
    void push(std::unique_ptr<Scene> scene);
    void pop();
    uint32_t num_scenes();
    Scene *get_scene() const;

    void set_camera(Camera *camera);
    
private:
    std::stack<std::unique_ptr<Scene>> m_scene_stack;
    Camera *m_camera = nullptr;
};