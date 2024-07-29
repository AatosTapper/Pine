#include "VertexBuffer.h"

#include "config.h"

VertexBuffer::VertexBuffer() noexcept {
    glGenBuffers(1, &m_id);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_id);
}

void VertexBuffer::set_data(const void *data, unsigned int size) {
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}