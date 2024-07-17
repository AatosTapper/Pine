#pragma once

#include "pch.h"
#include "Scene.h"
#include "rendering/Camera.h"

#include <stack>

class SceneManager {
    friend class Application;
public:
    void push(std::unique_ptr<Scene> scene);
    void pop();
    uint32_t num_scenes();
    Scene *get_scene() const;

    void set_camera(Camera *camera);
    
    void update();
    void try_update();

private:
    struct ManagerCommand {
        enum class Type {
            Push = 0,
            Pop
        } type;
        std::unique_ptr<Scene> scene;
    };

    void m_push(std::unique_ptr<Scene> &&scene);
    void m_pop();

    std::stack<std::unique_ptr<Scene>> m_scene_stack;
    std::vector<ManagerCommand> m_commands;
    Camera *m_camera = nullptr;
    bool m_update_whenever = true;
};