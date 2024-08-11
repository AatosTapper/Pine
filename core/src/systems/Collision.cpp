#include "systems/Collision.h"

#include "pch.h"
#include "scene/Components.h"
#include "scene/Entity.h"

using component::Collider;
using component::Transform;

enum class CollisionType : uint8_t {
    BoxBox = 1,
    CircleCircle = 2,
    BoxCircle = 4
};

struct CollisionCheck {
    Transform &t1;
    Transform &t2;
    Collider &c1;
    Collider &c2;
    CollisionType type;
};

static CollisionCheck format_collision(const entt::entity e1, const entt::entity e2, Scene *const scene) {
    Entity entity_1 { e1, scene };
    Entity entity_2 { e2, scene };
    auto &&[collider_1, transform_1] = entity_1.get<Collider, Transform>();
    auto &&[collider_2, transform_2] = entity_2.get<Collider, Transform>();

    auto create_collision_check = [] (Transform &t1, Transform &t2, Collider &c1, Collider &c2, CollisionType type) {
        return CollisionCheck {
            .t1 = t1,
            .t2 = t2,
            .c1 = c1,
            .c2 = c2,
            .type = type
        };
    };

    switch (collider_1.type) {
        case Collider::Type::AABB: {
            switch (collider_2.type) {
                case Collider::Type::AABB: {
                    return create_collision_check(transform_1, transform_2, collider_1, collider_2, CollisionType::BoxBox);
                }
                case Collider::Type::Circle: {
                    return create_collision_check(transform_1, transform_2, collider_1, collider_2, CollisionType::BoxCircle);
                }
            }
        }
        case Collider::Type::Circle: {
            switch (collider_2.type) {
                case Collider::Type::AABB: {
                    return create_collision_check(transform_2, transform_1, collider_2, collider_1, CollisionType::BoxCircle);
                }
                case Collider::Type::Circle: {
                    return create_collision_check(transform_1, transform_2, collider_1, collider_2, CollisionType::CircleCircle);
                }
            }
        }
    }
    std::cerr << "Collision error: no matching collision type for objects\n";
    std::abort();
}

static std::optional<glm::vec2> box_box_intersects(const CollisionCheck &check) {
    const auto &t1 = check.t1;
    const auto &t2 = check.t2;

    const double a_l = t1.x - (double)t1.sx;
    const double a_r = t1.x + (double)t1.sx;
    const double b_l = t2.x - (double)t2.sx;
    const double b_r = t2.x + (double)t2.sx;

    const double a_t = t1.y + (double)t1.sy;
    const double a_b = t1.y - (double)t1.sy;
    const double b_t = t2.y + (double)t2.sy;
    const double b_b = t2.y - (double)t2.sy;

    const double left = b_l - a_r;
    const double right = b_r - a_l;
    const double bottom = b_b - a_t;
    const double top = b_t - a_b;

    if (left > 0.0 || right < 0.0 || bottom > 0.0 || top < 0.0) {
        return std::nullopt;
    }

    glm::vec2 mtv;
    if (std::abs(left) < std::abs(right)) {
        mtv.x = left;
    } else {
        mtv.x = right;
    }
    if (std::abs(bottom) < std::abs(top)) {
        mtv.y = bottom;
    } else {
        mtv.y = top;
    }
    if (std::abs(mtv.x) < std::abs(mtv.y)) {
        mtv.y = 0;
    } else {
        mtv.x = 0;
    }

    return mtv;
}

static std::optional<glm::vec2> circle_circle_intersects(const CollisionCheck &check) {
    const auto &t1 = check.t1;
    const auto &t2 = check.t2;

    const float circle1_radius = std::max(t1.sx, t1.sy);
    const float circle2_radius = std::max(t2.sx, t2.sy);

    const float diff_x = (float)(t2.x - t1.x);
    const float diff_y = (float)(t2.y - t1.y);

    const float distance_squared = diff_x * diff_x + diff_y * diff_y;
    const float radius_sum = circle1_radius + circle2_radius;
    
    if (distance_squared < (radius_sum * radius_sum)) {
        if (distance_squared == 0) {
            return glm::vec2(circle1_radius, 0.0f);
        }
        return glm::normalize(glm::vec2(diff_x, diff_y)) * -(radius_sum - std::sqrt(distance_squared));
    }
    return std::nullopt;
}

static std::optional<glm::vec2> box_circle_intersects(const CollisionCheck &check) {
    const auto &t1 = check.t1;
    const auto &t2 = check.t2;

    const glm::vec2 aabb_half_extents(t1.sx, t1.sy);
    const glm::vec2 aabb_center(t1.x, t1.y);

    const float circle_radius = std::max(t2.sx, t2.sy);
    const glm::vec2 circle_center(t2.x, t2.y);

    glm::vec2 difference = circle_center - aabb_center;
    const glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    const glm::vec2 closest = aabb_center + clamped;
    difference = closest - circle_center;

    const float distance = glm::length(difference);
    if (distance < circle_radius) {
        if (distance == 0.0f) {
            return glm::vec2(circle_radius, 0.0f);
        }
        return glm::normalize(difference) * (circle_radius - glm::length(difference));
    }
    return std::nullopt;
}

static std::optional<glm::vec2> intersects(const CollisionCheck &check) {
    switch (check.type) {
        case CollisionType::BoxBox: {
            return box_box_intersects(check);
        } case CollisionType::CircleCircle: {
            return circle_circle_intersects(check);
        } case CollisionType::BoxCircle: {
            return box_circle_intersects(check);
        }
    }
    std::unreachable();
}

static void solve_intersection(const glm::vec2 &normal, CollisionCheck &check) {
    const auto &c1 = check.c1;
    const auto &c2 = check.c2;
    auto &t1 = check.t1;
    auto &t2 = check.t2;
    if (c1.resolve_collisions && c2.resolve_collisions) {
        static constexpr float extra = 1.01f;
        float mult1 = 0.5f;
        float mult2 = 0.5f;
        if (c1.fixed || c2.fixed) {
            if (!c1.fixed) {
                mult1 = 0.0f;
                mult2 = 1.0f;
            } else if (!c2.fixed) {
                mult1 = 0.0f;
                mult2 = 1.0f;
            }
        }
        t1.x += normal.x * mult1 * extra;
        t2.x -= normal.x * mult2 * extra;
        t1.y += normal.y * mult1 * extra;
        t2.y -= normal.y * mult2 * extra;
    }
}

std::optional<glm::vec2> collide(const entt::entity e1, const entt::entity e2, Scene *const scene) {
    CollisionCheck check = format_collision(e1, e2, scene);
    if (auto result = intersects(check)) {
        glm::vec2 normal = *result;
        solve_intersection(normal, check);
        return normal;
    }
    return std::nullopt;
}