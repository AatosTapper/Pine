#pragma once

#include "pch.h"
#include "config.h"

#include "events/Event.h"

class Window {
public:
    struct Data {
        int width = 0;
        int heigth = 0;
        EventCallback<Event> callback;
    };

    Window(int width, int height, const char *name);
    ~Window();

    GLFWwindow *get_glfw_window() const { return m_window; }
    bool is_open() const { return !glfwWindowShouldClose(m_window); }
    void update();

    glm::i32vec2 get_dimensions() const;
    int get_width() const { return m_data.width; }
    int get_height() const { return m_data.heigth; }
    float get_aspect_ratio() const { return (float)m_data.width / (float)m_data.heigth; }

    void set_event_callback(EventCallback<Event> fn) { 
        m_data.callback = fn;
        m_fix_startup_framebuffer();
    }

private:
    Data m_data;
    GLFWwindow *m_window = nullptr;
    std::function<void(GLFWwindow *window, int _width, int _height)> m_resize_callback;

    void m_fix_startup_framebuffer() {
        int fb_width, fb_height;
        glfwGetFramebufferSize(m_window, &fb_width, &fb_height);
        //glViewport(0, 0, fb_width, fb_height);

        m_resize_callback(m_window, fb_width, fb_height);
    }
};