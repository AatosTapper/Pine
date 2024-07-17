#pragma once

#include "pch.h"
#include "Scene.h"

void custom_behaviour_system_update(Scene *scene);

void update_transform_component_last_states(Scene *scene);
void interpolate_transform_components(Scene *scene, float alpha);