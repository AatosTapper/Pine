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
    Entity entity_1{ e1, scene };
    Entity entity_2{ e2, scene };
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

void collide(const entt::entity e1, const entt::entity e2, Scene *const scene) {
    CollisionCheck check = format_collision(e1, e2, scene);
    switch (check.type) {
        case CollisionType::BoxBox: {
            
        } break;
        case CollisionType::CircleCircle: {
            
        } break;
        case CollisionType::BoxCircle: {
            
        } break;
    }
}