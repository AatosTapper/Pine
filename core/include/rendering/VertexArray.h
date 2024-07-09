#pragma once

#include "singleton.h"

class VertexBuffer;
class VertexBufferLayout;
class VertexArray {
public:
    VertexArray();
    ~VertexArray();
    IMPL_NO_COPY(VertexArray)

    void add_buffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

    void bind() const;
    void unbind() const;

    unsigned int get_id() const { return m_id; }
    
private:
    unsigned int m_id = 0;
};