#pragma once


#include "config.h"

#include "events/Event.h"

class Window {
public:
    Window(int width, int height, const char *name);
    ~Window();

    GLFWwindow *get_glfw_window() const { return m_window; }
    bool is_open() const { return !glfwWindowShouldClose(m_window); }
    void update();

    glm::vec2 get_dimensions() const;
    int get_width() const { return m_data.width; }
    int get_height() const { return m_data.heigth; }
    float get_aspect_ratio() const { return (float)m_data.width / (float)m_data.heigth; }

    void set_event_callback(EventCallback<Event> fn) { m_data.callback = fn; }

private:
    GLFWwindow *m_window = nullptr;

    struct Data {
        int width = 0;
        int heigth = 0;
        EventCallback<Event> callback;
    } m_data;
};