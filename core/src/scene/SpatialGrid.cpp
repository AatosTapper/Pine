#include "scene/SpatialGrid.h"

#include "scene/Scene.h"
#include "scene/Components.h"
#include "scene/Entity.h"

static float get_closest_distance(const component::Transform &oth, float x, float y) {
    const float half_width = oth.sx / 2.0f;
    const float half_height = oth.sy / 2.0f;

    const float left = oth.x - half_width;
    const float right = oth.x + half_width;
    const float top = oth.y + half_height;
    const float bottom = oth.y - half_height;

    float dx = 0.0f;
    float dy = 0.0f;
    
    if (x < left) {
        dx = left - x;
    } else if (x > right) {
        dx = x - right;
    }

    if (y < bottom) {
        dy = bottom - y;
    } else if (y > top) {
        dy = y - top;
    }

    return std::sqrt(dx * dx + dy * dy);
}

std::vector<entt::entity> SpatialGrid::spatial_lookup(double x, double y, float radius, Scene *scene) {
    std::vector<entt::entity> results;

    const uint32_t center_index = m_calc_index(x, y);
    const int32_t radius_cells = static_cast<int32_t>(std::ceil(radius / m_cell_size));

    for (int32_t dx = -radius_cells; dx <= radius_cells; dx++) {
        for (int32_t dy = -radius_cells; dy <= radius_cells; dy++) {
            const uint32_t index = center_index + dx + dy * (m_half_width * 2);

            if (m_grid.find(index) == m_grid.end()) continue;

            for (auto ent : m_grid[index]) {
                auto &oth = scene->get_registry()->get<component::Transform>(ent);
                const float dist = get_closest_distance(oth, x, y);
                if ((dist - radius) < 0.0f) [[likely]] {
                    results.push_back(ent);
                }
            }
        }
    }

    std::sort(results.begin(), results.end());
    results.resize(std::distance(results.begin(), std::unique(results.begin(), results.end())));

    return results;
}

void SpatialGrid::for_all_cells(std::function<void (const std::vector<entt::entity>&)> predicate) {
    for (const auto &pair : m_grid) {
        predicate(pair.second);
    }
}

void SpatialGrid::push_entity(Entity entity) {
    const Scene *const scene = entity.get_scene();
    const entt::entity ent = entity.get_entt();

    std::vector<uint32_t> limits;
    limits.reserve(4);

    const auto &t = scene->get_registry()->get<component::Transform>(ent);

    const double left = t.x - t.sx / 2.0;
    const double right = t.x + t.sx / 2.0;
    const double top = t.y + t.sy / 2.0;
    const double bottom = t.y - t.sy / 2.0;

    limits.emplace_back(m_calc_index(left, top));
    limits.emplace_back(m_calc_index(left, bottom));
    limits.emplace_back(m_calc_index(right, bottom));
    limits.emplace_back(m_calc_index(right, top));

    std::sort(limits.begin(), limits.end());
    auto unique_end = std::unique(limits.begin(), limits.end());

    for (auto it = limits.begin(); it != unique_end; ++it) {
        m_grid[*it].push_back(ent);
    }
}

void SpatialGrid::populate(Scene *scene) {
    m_grid.clear();
    const auto view = scene->get_view<component::Transform>();
    std::vector<uint32_t> limits;
    limits.reserve(4);
    for (const auto ent : view) {
        const auto &t = scene->get_registry()->get<component::Transform>(ent);

        const double left = t.x - t.sx / 2.0;
        const double right = t.x + t.sx / 2.0;
        const double top = t.y + t.sy / 2.0;
        const double bottom = t.y - t.sy / 2.0;

        limits.emplace_back(m_calc_index(left, top));
        limits.emplace_back(m_calc_index(left, bottom));
        limits.emplace_back(m_calc_index(right, bottom));
        limits.emplace_back(m_calc_index(right, top));

        std::sort(limits.begin(), limits.end());
        auto unique_end = std::unique(limits.begin(), limits.end());

        for (auto it = limits.begin(); it != unique_end; ++it) {
            m_grid[*it].push_back(ent);
        }

        limits.clear();
    }    
}

uint32_t SpatialGrid::m_calc_index(double x, double y) {
    const double max_x = m_half_width * m_cell_size;
    const double max_y = m_half_height * m_cell_size;

    const double grid_x = x + max_x;
    const double grid_y = y + max_y;

    if (grid_x < 0.0 || grid_y < 0.0 || grid_x >= max_x * 2.0 || grid_y >= max_y * 2.0) {
        std::cout << "Warning: Entity passed the spatial grid border\n(entity is too far from origin)\nThis may cause undefined behavior\n";
        return 0;
    }

    const uint32_t cell_x = static_cast<uint32_t>(grid_x / m_cell_size);
    const uint32_t cell_y = static_cast<uint32_t>(grid_y / m_cell_size);

    return cell_x + cell_y * (m_half_width * 2);
}