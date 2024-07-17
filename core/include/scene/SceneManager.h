#pragma once

#include "pch.h"
#include "Scene.h"
#include "rendering/Camera.h"

#include <deque>

class SceneManager {
    friend class Application;
public:
    void push(std::unique_ptr<Scene> scene);
    void pop();
    uint32_t num_scenes() const;
    Scene *get_scene() const;

    void set_camera_data(CameraData &&data);
    void cam_aspect_ratio_callback(float aspect_ratio);
    
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

    std::deque<std::unique_ptr<Scene>> m_scene_stack;
    std::vector<ManagerCommand> m_commands;
    CameraData m_camera_data{};
    bool m_restrict_updates = false;
};