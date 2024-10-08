#include "Window.h"

#include "pch.h"
#include "events/WindowEvent.h"
#include "events/KeyEvent.h"
#include "events/MouseEvent.h"
#include "profiling.h"

static void glfw_error_callback(int error, const char *description) {
    std::cout << "GLFW Error: [" << error << "]: " << description << std::endl;
}

Window::Window(int width, int height, const char *name) noexcept {
    if (!glfwInit()) {
        std::cout << "Failed to init GLFW\n";
        assert(false);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4); // anti-aliasing (MSAA)
    glfwSwapInterval(0); 

    // some apple shit
#ifdef __APPLE__
    std::cout << "Apple compatability on\n";
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (m_window == NULL) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        std::abort();
    }
    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        std::abort();
    }
    
    glfwSetWindowUserPointer(m_window, &m_data);

    auto resize_callback = [](GLFWwindow *window, int _width, int _height) {
        Data *data = static_cast<Data*>(glfwGetWindowUserPointer(window));
        data->width = _width;
        data->heigth = _height;

        WindowResizeEvent event(_width, _height);
        data->callback(&event);
    };
    glfwSetWindowSizeCallback(m_window, resize_callback);

    auto framebuffer_size_callback = [](GLFWwindow *window, int fb_width, int fb_height) {
        (void)window;
        glViewport(0, 0, fb_width, fb_height);
    };
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow *window) {
        Data *data = static_cast<Data*>(glfwGetWindowUserPointer(window));
        WindowCloseEvent event;
        data->callback(&event);
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        Data *data = static_cast<Data*>(glfwGetWindowUserPointer(window));
        (void)scancode; 
        (void)mods;
        switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key);
                data->callback(&event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                data->callback(&event);
                break;
            }
            case GLFW_REPEAT: {
                KeyRepeatEvent event(key);
                data->callback(&event);
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow *window, int button, int action, int mods) {
        Data *data = static_cast<Data*>(glfwGetWindowUserPointer(window));
        (void)mods;
        switch (action) {
            case GLFW_PRESS: {
                double x, y;
                glfwGetCursorPos(window, &x, &y);
                MouseButtonPressedEvent event(button, x, y);
                data->callback(&event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                data->callback(&event);
                break;
            }
        }
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, double x, double y) {
        Data *data = static_cast<Data*>(glfwGetWindowUserPointer(window));
        MouseMovedEvent event(x, y);
        data->callback(&event);
    });

    glfwSetErrorCallback(glfw_error_callback);

    int fb_width = 0, fb_height = 0;
    glfwGetFramebufferSize(m_window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);
}

Window::~Window() {
    glfwTerminate();
}

void Window::update() {
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

glm::i32vec2 Window::get_dimensions() const {
    return glm::i32vec2(m_data.width, m_data.heigth);
}

glm::i32vec2 Window::get_framebuffer_dimensions() const {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return glm::i32vec2(width, height);
}