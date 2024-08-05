#include "scene/SceneParser.h"

#include <fstream>
#include <sstream>

#include <tinyxml2/tinyxml2.h>

using namespace tinyxml2;

static SceneNodeType name_to_type(const char* name) {
    if (strcmp(name, "Scene") == 0) {
        return SceneNodeType::Scene;
    } if (strcmp(name, "Ent") == 0) {
        return SceneNodeType::Entity;
    } if (strcmp(name, "Cmp") == 0) {
        return SceneNodeType::Component;
    } if (strcmp(name, "Data") == 0) {
        return SceneNodeType::Data;
    } else {
        return SceneNodeType::Undefined;
    }
}

static std::string type_to_name(SceneNodeType name) {
    switch (name) {
    case SceneNodeType::Undefined:
        return "Undefined";
    case SceneNodeType::Scene:
        return "Scene";
    case SceneNodeType::Entity:
        return "Ent";
    case SceneNodeType::Component:
        return "Cmp";
    case SceneNodeType::Data:
        return "Data";
    }
}

void SceneNode::print_node(uint32_t indentation) {
    std::string indent(indentation * 2, ' ');
    std::cout << indent << type_to_name(data.type) << std::endl;
    std::cout << indent << "variables [" << std::endl;
    for (const auto &var : data.variables) {
        std::cout << indent << "  " << var.name << "=" << var.value << std::endl;
    }
    std::cout << indent << "]" << std::endl;
}

void SceneNode::print_tree(uint32_t depth) {
    print_node(depth);
    for (auto &child : children) {
        child->print_tree(depth + 1);
    }
}

static XMLElement *create_element(std::unique_ptr<SceneNode> &node, XMLDocument &doc) {
    std::string name = type_to_name(node->data.type);
    XMLElement *element = doc.NewElement(name.c_str());
    for (const auto &var : node->data.variables) {
        element->SetAttribute(var.name.c_str(), var.value.c_str());
    }
    for (auto &child : node->children) {
        element->InsertEndChild(create_element(child, doc));
    }
    return element;
}

void write_scene(std::unique_ptr<SceneNode> &scene, const char *filepath) {
    XMLDocument xml_doc;
    xml_doc.InsertFirstChild(create_element(scene, xml_doc));
    auto result = xml_doc.SaveFile(filepath);
    if (result != XML_SUCCESS) {
        std::cerr << "Error saving file: " << result << std::endl;
        assert(false);
    }
}

static std::unique_ptr<SceneNode> parse_xml(const XMLElement* element, SceneNode *parent, int depth = 0) {
    if (element == nullptr) return nullptr;

    auto node = std::make_unique<SceneNode>();
    node->data.type = name_to_type(element->Name());

    if (node->data.type == SceneNodeType::Undefined) {
        if (parent == nullptr) {
            std::cout << "Error: undefined type for root node." << std::endl;
        } else {
            std::cout << "Error: undefined type on line " << element->GetLineNum() << std::endl;
        }
        std::abort();
    }

    for (const XMLAttribute* attribute = element->FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
        node->data.variables.push_back({ attribute->Name(), attribute->Value() });
    }
    for (const XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        node->children.push_back(parse_xml(child, node.get(), depth + 1));
    }

    return node;
}

std::unique_ptr<SceneNode> read_scene(const char* file) {
    XMLDocument doc;
    XMLError result = doc.LoadFile(file);
    if (result != XML_SUCCESS) {
        std::cerr << "Error loading file: " << result << std::endl;
        assert(false);
    }

    const XMLElement* root = doc.FirstChildElement();
    if (root == nullptr) {
        std::cerr << "Error finding root element." << std::endl;
        assert(false);
    }
    return parse_xml(root, nullptr);
}