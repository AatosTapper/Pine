#pragma once

#include "pch.h"

class Texture {
public:
    Texture(const std::string &file); // flip parameter is for certain file formats
    ~Texture();
    Texture(const Texture& other) {
        m_id = other.m_id;
    }
    Texture(Texture&& other) {
        m_id = other.m_id;
    }

    void bind() const;
    void unbind() const;

    void filter_nearest(); // pixelated look
    void filter_linear();  // normal filtering

private:
    unsigned int m_id = 0;
};
