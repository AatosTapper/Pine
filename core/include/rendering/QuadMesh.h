#pragma once

#include "pch.h"
#include "config.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class QuadMesh {
public:
    QuadMesh(float width, float height);
    QuadMesh(float size) : QuadMesh(size, size) {}
    QuadMesh() : QuadMesh(1.0f, 1.0f) {}

    VertexArray *get_vao() const;
    IndexBuffer *get_ebo() const;
    
    glm::mat4 transform;
};