#pragma once

#include "config.h"
#include "rendering/Shader.h"
#include "singleton.h"
#include "QuadMesh.h"
#include "Window.h"
#include "scene/Scene.h"

#include "pch.h"

class Shader;

class Renderer {
public:
    Renderer();
    ~Renderer();

    IMPL_NO_COPY(Renderer)

    void init();
    void start_frame();
    void draw_frame(Scene *scene);
    void regenerate_framebuffer();

    void set_shader(Shader *shader) { assert(shader); m_selected_shader = shader; }
    void set_view_proj_matrix(const glm::mat4 &vp_mat) { m_selected_vpm = vp_mat; }
    void set_window_dimensions(glm::i32vec2 data) { m_window_dimensions = data; }

private:
    Shader m_sprite_shader;
    Shader m_post_process_shader;
    
    Shader *m_selected_shader = nullptr;
    glm::mat4 m_selected_vpm {};
    glm::i32vec2 m_window_dimensions {};

    uint32_t m_framebuffer = 0;
    uint32_t m_texture_color_buffer = 0;
    uint32_t m_texture_depth_buffer = 0;
    uint32_t m_rbo = 0;
    QuadMesh m_screen_quad;
    
    void m_draw_sprites(Scene *scene);

    void m_create_framebuffers();
    void m_delete_framebuffers();
    void m_render_framebuffer();
};
