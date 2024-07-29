#include "IndexBuffer.h"

#include "config.h"

IndexBuffer::IndexBuffer() noexcept : m_elements(0) {
     glGenBuffers(1, &m_id);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_id);
}

void IndexBuffer::set_data(const unsigned int *data, unsigned int count) {
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
    m_elements = count;
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}