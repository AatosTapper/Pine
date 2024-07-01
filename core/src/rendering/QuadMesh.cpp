#include "QuadMesh.h"

#include "VertexBufferLayout.h"

static bool mesh_created = false;
static std::unique_ptr<VertexArray> vao_memory = nullptr;
static std::unique_ptr<IndexBuffer> ebo_memory = nullptr;
static std::unique_ptr<VertexBuffer> vbo_memory = nullptr;

static constexpr float vertices[] = {
     1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
     1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,     0.0f, 1.0f
};

static constexpr uint32_t indices[] = {
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

QuadMesh::QuadMesh(float width, float height) {
    transform = glm::scale(transform, glm::vec3(width, height, 1.0));

    if (!mesh_created) {
        mesh_created = true;

        vao_memory = std::make_unique<VertexArray>();
        vbo_memory = std::make_unique<VertexBuffer>();
        ebo_memory = std::make_unique<IndexBuffer>();

        VertexBufferLayout layout;
        layout.push<float>(3); // position
        layout.push<float>(2); // texture coordinates

        vbo_memory->set_data(vertices, sizeof(vertices));
        ebo_memory->set_data(indices, 6);
        vao_memory->add_buffer(*vbo_memory, layout);
    }

    m_vao = vao_memory.get();
    m_vbo = vbo_memory.get();
    m_ebo = ebo_memory.get();
}