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
    m_instanced_sprite_shader(Shader("../res/shaders/sprite_instanced.vert", "../res/shaders/sprite_instanced.frag")),
    m_post_process_shader(Shader("../res/shaders/post_process.vert", "../res/shaders/post_process.frag")),
    m_selected_shader(nullptr)
{
    glGenBuffers(1, &m_batch_buffer);
}

Renderer::~Renderer() {
    m_delete_framebuffers();
    if (m_batch_buffer != 0) {
        glDeleteBuffers(1, &m_batch_buffer);
    }
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
    m_draw_sprites_instanced(scene);
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
        
        if (sprite.img == nullptr) continue;

        m_selected_shader->use();

        glActiveTexture(GL_TEXTURE0);
        sprite.img->bind();

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

struct Batch {
    std::vector<glm::mat4> transforms;
    Texture *texture;
};

static std::vector<Batch> create_render_batches(Scene *scene) {
    std::vector<Batch> batches;
    float z_offset = 0.0f;

    for (auto &ent : scene->get_view<component::Sprite>()) {
        auto [sprite, transform] = Entity(ent, scene).get<component::Sprite, component::Transform>();
        if (sprite.img == nullptr) [[unlikely]] continue;

        z_offset += 0.0002f;
        glm::vec3 offset = glm::vec3(0, 0, z_offset + sprite.render_layer);

        const auto it = std::find_if(batches.begin(), batches.end(), [&sprite](const Batch &oth) {
            return oth.texture == sprite.img;
        });
        if (it != batches.end()) {
            it->transforms.emplace_back(glm::translate(transform.get_matrix(), offset));
        } else {
            batches.emplace_back(Batch{ .texture = sprite.img });
            batches.back().transforms.emplace_back(glm::translate(transform.get_matrix(), offset));
        }
    }
    return batches;
}

void Renderer::m_draw_sprites_instanced(Scene *scene) {
    if (m_selected_shader == nullptr) {
        m_selected_shader = &m_instanced_sprite_shader;
    }

    auto batches = create_render_batches(scene);
    auto &mesh = QuadMesh::instance();

    m_selected_shader->use();
    m_selected_shader->set_mat4f("u_view_proj", m_selected_vpm);

    mesh.get_vao()->bind();
    mesh.get_ebo()->bind();
    // Set up the vertex attribute pointers for the matrix buffer
    for (uint32_t i = 0; i < 4; i++) {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(2 + i, 1);
    }
    for (const auto &batch : batches) {
        auto &matrices = batch.transforms;

        glActiveTexture(GL_TEXTURE0);
        batch.texture->bind();

        glBindBuffer(GL_ARRAY_BUFFER, m_batch_buffer);
        glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &matrices[0], GL_STATIC_DRAW);

        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.get_ebo()->get_elements()), GL_UNSIGNED_INT, 0, matrices.size());
    }
    mesh.get_ebo()->unbind();
    mesh.get_vao()->unbind();
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

    glGenTextures(1, &m_texture_color_buffer);
    glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 
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
        std::cerr << "Error creating framebuffer: ";
        switch (status) {
            case GL_FRAMEBUFFER_UNDEFINED:
                std::cerr << "GL_FRAMEBUFFER_UNDEFINED\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n";
                break;
            default:
                std::cerr << "Unknown framebuffer error\n";
                break;
        }
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