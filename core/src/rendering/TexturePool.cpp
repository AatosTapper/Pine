#include "rendering/TexturePool.h"

Texture *TexturePool::get(const std::string &path) {
    auto it = std::find_if(m_pool.begin(), m_pool.end(), [&path](TextureWrapper &item) {
        return item.name == path;
    });
    if (it != m_pool.end()) [[likely]] {
        return it->texture.get();
    }
    m_pool.push_back(TextureWrapper{ .name = path, .texture = std::make_unique<Texture>(path) });
    return m_pool.back().texture.get();
}