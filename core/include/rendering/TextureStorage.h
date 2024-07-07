#pragma once

#include "pch.h"
#include "Texture.h"

class TextureStorage {
public:
    TextureStorage();

    using tex_id = uint32_t; // 0 = default
    tex_id push(const std::string &path);
    Texture &get(tex_id id) { return m_storage.at(id);  }

private:
    std::vector<Texture> m_storage;
    std::unordered_map<std::string, tex_id> m_query;
};