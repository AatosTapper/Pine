#pragma once

#include "scene/Scene.h"

std::optional<glm::vec2> collide(const entt::entity e1, const entt::entity e2, Scene *const scene);