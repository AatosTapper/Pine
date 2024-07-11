#pragma once

#include "pch.h"
#include "config.h"
#include "singleton.h"

class VertexArray;
class IndexBuffer;
class QuadMesh {
public:
    QuadMesh() noexcept;

    IMPL_SINGLETON_DISPATCHER(QuadMesh)
    IMPL_NO_COPY(QuadMesh)

    VertexArray *get_vao() const;
    IndexBuffer *get_ebo() const;
};