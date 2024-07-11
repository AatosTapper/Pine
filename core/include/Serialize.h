#pragma once

#include <sstream>
#include <variant>

#include "SceneParser.h"
#include "profiling.h"

/// @todo OPTIMIZE:
///         Stringstream objects are created for each variable when read and written to.
///         Surely something more can be done too.

namespace serialize_internals {
    template <typename T> inline std::string to_string(const T& var);
    template <typename T> inline void var_from_node(T *var, const char *name, NodeData &data);
}

#define CHECK_SERDE(checks) assert(checks);

/// @brief create a NodeVariable struct from a C++ variable
#define VAR_TO_NODE(var) NodeVariable{ .name=#var, .value=serialize_internals::to_string(var) }

/// @brief find and get C++ variable from a NodeData struct
#define VAR_FROM_NODE(var, data) serialize_internals::var_from_node(&var, #var, data)

struct Serializable {
    virtual ~Serializable() = default;

    virtual void deserialize(NodeData &data) = 0;   // from file
    virtual NodeData serialize() const = 0;         // to file

    void test_serde() { 
        Profiler("Test serialization");
        NodeData data = serialize(); 
        deserialize(data);
    }
};

namespace serialize_internals {

template <typename T>
inline std::string to_string(const T &var) {
    std::ostringstream oss;
    oss << var;
    return oss.str();
}

template <>
inline std::string to_string<std::string>(const std::string &var) {
    return var;
}

template <>
inline std::string to_string<const char*>(const char *const &var) {
    return std::string(var);
}

template <typename T>
inline std::string to_string(const std::vector<T> &vec) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << to_string(vec[i]);
        oss << " ";
    }
    return oss.str();
}

template <typename T> 
inline std::vector<T> vec_from_string(const std::string &str) {
    std::istringstream ss(str);
    std::vector<T> output;
    T element;
    while (ss >> element) { 
        output.push_back(element);
    }
    return output;
}

template <typename T> 
inline T from_string(const std::string &str) {
    std::istringstream ss(str);
    T element;
    ss >> element;
    return element;
}

template <typename T>
inline void var_from_node(T *var, const char *name, NodeData &data) {
    auto it = std::find_if(data.variables.begin(), data.variables.end(), [&name](NodeVariable &variable) {
        return variable.name == name;
    });
    if (it == data.variables.end()) {
        std::cerr << "Deserialize error: variable \"" << name << "\" is not defined in the scene file.\n";
    }
    *var = from_string<T>(it->value);
}

template <typename T>
inline void var_from_node(std::vector<T> *var, const char *name, NodeData &data) {
    auto it = std::find_if(data.variables.begin(), data.variables.end(), [&name](NodeVariable &variable) {
        return variable.name == name;
    });
    if (it == data.variables.end()) {
        std::cerr << "Deserialize error: variable \"" << name << "\" is not defined in the scene file.\n";
    }
    *var = vec_from_string<T>(it->value);
}

}