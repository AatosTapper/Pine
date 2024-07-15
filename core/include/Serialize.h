#pragma once

#include <sstream>

#include "pch.h"
#include "SceneParser.h"
#include "profiling.h"
#include "FromString.h"

/// @todo OPTIMIZE!

namespace serialize_internals {
    template <typename T> inline constexpr std::string to_string(const T& var);
    template <typename T> inline constexpr void var_from_node(T *var, const char *name, NodeData &data);
}

#define CHECK_SERDE(checks) assert(checks);

/// @brief set component's type variable
#define COMP_TYPE(type) NodeVariable{ .name="type", .value=#type }

/// @brief create a NodeVariable struct from a C++ variable
#define VAR_TO_NODE(var) NodeVariable{ .name=#var, .value=serialize_internals::to_string(var) }

/// @brief find and get C++ variable from a NodeData struct
#define VAR_FROM_NODE(var, data) serialize_internals::var_from_node(&var, #var, data)

struct Serializable {
    virtual ~Serializable() = default;

    virtual void deserialize(NodeData &data) = 0;   // from file
    virtual NodeData serialize() const = 0;         // to file

    void test_serde() { 
        Profiler p("Test serialization");
        NodeData data = serialize(); 
        deserialize(data);
    }
};
/// @brief A tag used to mark entities as non serializable
struct EntityDoNotSerialize {};

namespace serialize_internals {

template <typename T>
inline constexpr std::string to_string(const T &var) {
    std::ostringstream oss;
    oss << var;
    return oss.str();
}

template <>
inline constexpr std::string to_string<std::string>(const std::string &var) {
    return var;
}

template <>
inline constexpr std::string to_string<std::string_view>(const std::string_view &var) {
    return std::string(var);
}

template <>
inline constexpr std::string to_string<const char*>(const char *const &var) {
    return std::string(var);
}

template <typename T>
inline constexpr std::string to_string(const std::vector<T> &vec) {
    std::ostringstream oss;
    for (const auto &item : vec) {
        oss << to_string(item);
        oss << " ";
    }
    return oss.str();
}

template <typename T> 
inline constexpr std::vector<T> vec_from_string(const std::string &str) {
    std::istringstream ss(str);
    std::vector<T> output;
    T element;
    while (ss >> element) {
        output.push_back(element);
    }
    return output;
}

template <typename T>
inline constexpr auto find_var(const char *name, NodeData &data) {
    auto it = std::find_if(data.variables.begin(), data.variables.end(), [&name](NodeVariable &variable) {
        return variable.name == name;
    });
    if (it == data.variables.end()) {
        std::cerr << "Deserialize error: variable \"" << name << "\" is not defined in the scene file.\n";
    }
    return it;
}

template <typename T>
inline constexpr void var_from_node(T *var, const char *name, NodeData &data) {
    *var = from_string<T>(find_var<T>(name, data)->value);
}

template <typename T>
inline constexpr void var_from_node(std::vector<T> *var, const char *name, NodeData &data) {
    *var = vec_from_string<T>(find_var<T>(name, data)->value);
}

}