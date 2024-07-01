#pragma once

#include "pch.h"
#include "config.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

class QuadMesh {
public:
    QuadMesh(float width, float height);
    QuadMesh(float size) : QuadMesh(size, size) {}
    QuadMesh() : QuadMesh(1.0f, 1.0f) {}
    virtual ~QuadMesh() {}
    
    glm::mat4 transform;
    
    VertexArray *get_vao() const { return m_vao; }
    IndexBuffer *get_ebo() const { return m_ebo; }

    virtual std::shared_ptr<Texture> get_texture() const { 
        assert(false && "get_texture() not implemented"); 
        return nullptr; 
    };

private:
    VertexArray *m_vao = nullptr;
    IndexBuffer *m_ebo = nullptr;
    VertexBuffer *m_vbo = nullptr;  
};