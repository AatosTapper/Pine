#pragma once

#include "pch.h"
#include "config.h"

#include "events/Event.h"

class Window {
public:
    struct Data {
        int width = 1280;
        int heigth = 720;
        EventCallback<Event> callback;
    };

    Window(int width, int height, const char *name) noexcept;
    ~Window();

    GLFWwindow *get_glfw_window() const { return m_window; }
    bool is_open() const { return !glfwWindowShouldClose(m_window); }
    void update();

    glm::i32vec2 get_dimensions() const;
    glm::i32vec2 get_framebuffer_dimensions() const;
    int get_width() const { return m_data.width; }
    int get_height() const { return m_data.heigth; }
    float get_aspect_ratio() const { return (float)m_data.width / (float)m_data.heigth; }

    void set_event_callback(EventCallback<Event> fn) { 
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        glViewport(0, 0, width, height);
        glfwGetWindowSize(m_window, &width, &height);
        m_data = Data {
            .callback = fn,
            .width = width,
            .heigth = height
        };
    }

private:
    Data m_data;
    GLFWwindow *m_window = nullptr;
};