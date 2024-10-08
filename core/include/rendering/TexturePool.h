#pragma once

#include "pch.h"
#include "Texture.h"
#include "singleton.h"

class TexturePool {
public:
    IMPL_SINGLETON_DISPATCHER(TexturePool)
    IMPL_NO_COPY(TexturePool)

    [[nodiscard]] Texture *get(const std::string &path);

    size_t num_textures() const { return m_pool.size(); }
    
private:
    TexturePool() = default;

    struct TextureWrapper {
        std::string name;
        std::unique_ptr<Texture> texture;
    };
    std::vector<TextureWrapper> m_pool;
};