#pragma once

#include "singleton.h"

class VertexBuffer {
public:
    VertexBuffer();
    ~VertexBuffer();
    IMPL_NO_COPY(VertexBuffer)

    void set_data(const void *data, unsigned int size);
    void bind() const;
    void unbind() const;

    unsigned int get_id() const { return m_id; }

private:
    unsigned int m_id = 0;
};