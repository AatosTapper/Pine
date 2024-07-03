#pragma once

#include "pch.h"

struct NodeData;
struct Serializable {
    virtual ~Serializable() = default;

    virtual void deserialize(NodeData &data) = 0;   // from file
    virtual NodeData serialize() = 0;               // to file
};

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

struct NodeData {
    NodeType type = NodeType::Undefined;
    std::vector<NodeVariable> variables;
};

struct Node {
    NodeData data;
    Node *parent = nullptr;
    std::vector<std::unique_ptr<Node>> children;
    

    void print_node(uint32_t indentation = 0);
    void print_tree(uint32_t depth = 0);
};

std::unique_ptr<Node> read_scene(const char* file);