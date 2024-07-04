#pragma once

#include "pch.h"

class SceneManager;

void set_lua_entity(sol::state &lua);
void set_lua_components(sol::state &lua);
void set_lua_scene(sol::state &lua, SceneManager &manager);
