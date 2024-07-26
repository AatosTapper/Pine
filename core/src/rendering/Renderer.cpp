#include "Renderer.h"

#include "Window.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "scene/Entity.h"

#include "scene/Components.h"

#include <stdexcept>

Renderer::Renderer() noexcept
  : m_sprite_shader(Shader("../res/shaders/sprite.vert", "../res/shaders/sprite.frag")), 
    m_post_process_shader(Shader("../res/shaders/post_process.vert", "../res/shaders/post_process.frag")),
    m_selected_shader(nullptr)
{
}

Renderer::~Renderer() {
    m_delete_framebuffers();
}

void Renderer::init() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_MULTISAMPLE);

    m_create_framebuffers();
}

void Renderer::start_frame() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::draw_frame(Scene *scene) {
    m_draw_sprites(scene);
    m_render_framebuffer();
}

void Renderer::m_draw_sprites(Scene *scene) {
    if (m_selected_shader == nullptr) {
        m_selected_shader = &m_sprite_shader;
    }

    auto &mesh = QuadMesh::instance();
    float z_offset = 0.0f; // remove z_fighting

    for (auto &ent : scene->get_view<component::Sprite>()) {
        auto [sprite, transform] = Entity(ent, scene).get<component::Sprite, component::Transform>();
        
        if (sprite.m_img == nullptr) continue;

        m_selected_shader->use();

        glActiveTexture(GL_TEXTURE0);
        sprite.m_img->bind();

        m_selected_shader->set_mat4f("u_view_proj", m_selected_vpm);
        m_selected_shader->set_mat4f("u_transform", transform);
        m_selected_shader->set_float("u_z_fight_factor", (z_offset + sprite.render_layer));

        mesh.get_vao()->bind();
        mesh.get_ebo()->bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.get_ebo()->get_elements()), GL_UNSIGNED_INT, 0);
        mesh.get_vao()->unbind();
        mesh.get_ebo()->unbind();

        z_offset += 0.0002f;
    }
}

void Renderer::m_render_framebuffer() {
    auto &mesh = QuadMesh::instance();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_post_process_shader.use();
    mesh.get_vao()->bind();
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);
    glUniform1i(glGetUniformLocation(m_post_process_shader.get_id(), "tex_col"), 0);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, m_texture_depth_stencil_buffer);
    glUniform1i(glGetUniformLocation(m_post_process_shader.get_id(), "tex_dep"), 1);

    mesh.get_vao()->bind();
    mesh.get_ebo()->bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.get_ebo()->get_elements()), GL_UNSIGNED_INT, 0);
    mesh.get_vao()->unbind();
    mesh.get_ebo()->unbind();
}

void Renderer::m_create_framebuffers() {
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // color buffer
    auto tex_format = GL_RGBA16F;

    glGenTextures(1, &m_texture_color_buffer);
    glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, tex_format, 
        m_window_dimensions.x, m_window_dimensions.y, 0,
        GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_color_buffer, 0);

    // combined depth-stencil buffer
    glGenTextures(1, &m_texture_depth_stencil_buffer);
    glBindTexture(GL_TEXTURE_2D, m_texture_depth_stencil_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
        m_window_dimensions.x, m_window_dimensions.y, 0,
        GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_texture_depth_stencil_buffer, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        switch (status) {
            case GL_FRAMEBUFFER_UNDEFINED:
                std::cerr << "GL_FRAMEBUFFER_UNDEFINED" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl;
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << std::endl;
                break;
            default:
                std::cerr << "Unknown framebuffer error" << std::endl;
                break;
        }
        std::cerr << "Framebuffer is not complete\n";
        std::abort();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::m_delete_framebuffers() {
    glDeleteTextures(1, &m_texture_depth_stencil_buffer);
    glDeleteTextures(1, &m_texture_color_buffer);
    glDeleteFramebuffers(1, &m_framebuffer);
}

void Renderer::regenerate_framebuffer() {
    m_delete_framebuffers();
    m_create_framebuffers();
}