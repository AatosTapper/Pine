#pragma once

#include "singleton.h"

class IndexBuffer {
public:
    IndexBuffer() noexcept;
    ~IndexBuffer();
    IMPL_NO_COPY(IndexBuffer)

    void set_data(const unsigned int *data, unsigned int count);
    void bind() const;
    void unbind() const;

    unsigned int get_elements() const { return m_elements; }

private:
    unsigned int m_id = 0;
    unsigned int m_elements = 0;
};