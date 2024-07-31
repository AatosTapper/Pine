#pragma once

#include "pch.h"
#include "Scene.h"
#include "singleton.h"
#include "SceneManager.h"

class SceneSerializer {
public:
    IMPL_NO_COPY(SceneSerializer)
    IMPL_SINGLETON_DISPATCHER(SceneSerializer)

    void save_current(SceneManager &manager);
    Scene *load_from_name(SceneManager &manager, const std::string &name);

    void serialize(Scene *scene, std::string path);
    std::unique_ptr<Scene> deserialize(std::string path);

    std::unique_ptr<SceneNode> serialize_entity(Entity entity);
    Entity deserialize_entity(Scene *scene, const std::unique_ptr<SceneNode> &node);

private:
    SceneSerializer() = default;
};