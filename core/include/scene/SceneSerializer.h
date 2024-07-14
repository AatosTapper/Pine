#pragma once

#include "pch.h"
#include "Scene.h"
#include "singleton.h"

class SceneSerializer {
public:
    IMPL_NO_COPY(SceneSerializer)
    IMPL_SINGLETON_DISPATCHER(SceneSerializer)

    void serialize(Scene *scene, std::string path);
    std::unique_ptr<Scene> deserialize(std::string path);

private:
    SceneSerializer() = default;
};