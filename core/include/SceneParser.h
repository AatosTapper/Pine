#pragma once

#include "pch.h"

struct NodeVariable {
    std::string name;
    std::string value;
};

enum class NodeType {
    Undefined = 0,
    Scene,
    Entity,
    Component,
    System
};

struct Node {
    NodeType type = NodeType::Undefined;
    Node *parent = nullptr;
    std::vector<std::unique_ptr<Node>> children;
    std::vector<NodeVariable> variables;

    void print_node(uint32_t indentation = 0);
    void print_tree(uint32_t depth = 0);
};

std::unique_ptr<Node> read_scene(const char* file);