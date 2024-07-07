#include "TextureStorage.h"

#include "ScriptEngine.h"

TextureStorage::TextureStorage() {
    push("../res/textures/default.png");
}

TextureStorage::tex_id TextureStorage::push(const std::string &path) {
    const auto index = m_storage.size();
    if (m_query.find(path) == m_query.end()) {
        m_query[path] = index;

        Texture tex(path);
        tex.filter_nearest();
        m_storage.push_back(std::move(tex));
        std::cout << "new texture created" << std::endl;
    }
    return index;
}