#pragma once

#include "pch.h"

struct SceneNode;
void write_scene(std::unique_ptr<SceneNode> &scene, const char *filepath);
std::unique_ptr<SceneNode> read_scene(const char* file);

struct SceneNodeVariable {
    std::string name;
    std::string value;
};

enum class SceneNodeType {
    Undefined = 0,
    Scene,
    Entity,
    Component,
    System
};

struct SceneNodeData {
    SceneNodeType type = SceneNodeType::Undefined;
    std::vector<SceneNodeVariable> variables;
};

struct SceneNode {
    SceneNodeData data;
    SceneNode *parent = nullptr;
    std::vector<std::unique_ptr<SceneNode>> children;

    void print_node(uint32_t indentation = 0);
    void print_tree(uint32_t depth = 0);
};