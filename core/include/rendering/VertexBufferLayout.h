#pragma once

#include "config.h"
#include "pch.h"

#include <stdexcept>

struct VertexBufferElement  {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int get_size_of_type(unsigned int type) {
        switch (type) {
            case GL_FLOAT: return sizeof(GLfloat);
            case GL_UNSIGNED_INT: return sizeof(GLuint);
            case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
        }
        assert(false && "No implementation for a type found");
        return 0;
    }
};

class VertexBufferLayout {
public:
    VertexBufferLayout() noexcept : m_stride(0u) {}

    const std::vector<VertexBufferElement>& get_elements() const { return m_elements; }
    unsigned int get_stride() const { return m_stride; }

    template<typename T> void push(unsigned int count);
    template<> void push<float>(unsigned int count);
    template<> void push<unsigned int>(unsigned int count);
    template<> void push<unsigned char>(unsigned int count);

private:
    std::vector<VertexBufferElement> m_elements;
    unsigned int m_stride;
};

template<typename T>
inline void VertexBufferLayout::push(unsigned int count) {
    (void)count;
    static_assert("Type not implemented");
}

template<>
inline void VertexBufferLayout::push<float>(unsigned int count) {
    m_elements.push_back({ GL_FLOAT, count, GL_FALSE });
    m_stride += VertexBufferElement::get_size_of_type(GL_FLOAT) * count;
}

template<>
inline void VertexBufferLayout::push<unsigned int>(unsigned int count) {
    m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
    m_stride += VertexBufferElement::get_size_of_type(GL_UNSIGNED_INT) * count;
}

template<>
inline void VertexBufferLayout::push<unsigned char>(unsigned int count) {
    m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
    m_stride += VertexBufferElement::get_size_of_type(GL_UNSIGNED_BYTE) * count;
}
