#pragma once

#include "pch.h"
#include "singleton.h"

class Renderer;
class Texture {
    friend class Renderer;
public:
    Texture() noexcept = default;
    explicit Texture(const std::string &file) noexcept;
    ~Texture();

    void filter_nearest();
    void filter_linear();

private:
    unsigned int m_id = 0;

    // only renderer can call these
    void bind() const;
    void unbind() const;
};
