#pragma once

#include "pch.h"
#include "Scene.h"

void custom_behavior_system_update(Scene *const scene);

void update_transform_component_last_states(const Scene *const scene);
void interpolate_transform_components(const Scene *const scene, float alpha);

void collision_system_update(Scene *scene);