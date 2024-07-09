#include "VertexArray.h"

#include "config.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_id);
    glBindVertexArray(m_id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_id);
}

void VertexArray::add_buffer(const VertexBuffer &vb, const VertexBufferLayout &layout) {
    bind();
    vb.bind();
    const auto &elements = layout.get_elements();
    unsigned int offset = 0;

    for (unsigned int i = 0; i < elements.size(); i++) {
        const auto &element = elements[i];

        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, static_cast<GLint>(element.count), 
            element.type, element.normalized, 
            static_cast<GLsizei>(layout.get_stride()), (void*)(size_t)offset);

        offset += element.count * VertexBufferElement::get_size_of_type(element.type);
    }
}

void VertexArray::bind() const {
    glBindVertexArray(m_id);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}