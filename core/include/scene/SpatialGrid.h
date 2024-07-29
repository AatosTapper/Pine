#pragma once

#include "pch.h"
#include "entt/entity/entity.hpp"

class Entity;
class Scene;
class SpatialGrid {
public:
    /// Returns all entities in the cells within radius
    std::vector<entt::entity> spatial_lookup(double x, double y, float radius, Scene *scene);
    /// runs predicate for all cells with entities
    void for_all_cells(std::function<void (const std::vector<entt::entity>&)> predicate);

    void push_entity(Entity entity);
    void populate(Scene *scene);

private:
    uint32_t m_half_width = 1000; // how many cells in one x directions (note: total is double this)
    uint32_t m_half_height = 1000; // how many cells in one y directions (note: total is double this)
    float m_cell_size = 3.0; // single rectangle cell width and height

    std::unordered_map<uint32_t, std::vector<entt::entity>> m_grid;

    uint32_t m_calc_index(double x, double y);
};