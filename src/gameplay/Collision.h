#ifndef SUNDOWNDRIVE_GAMEPLAY_COLLISION_H
#define SUNDOWNDRIVE_GAMEPLAY_COLLISION_H

#include <glm/glm.hpp>

// ── Collision ────────────────────────────────────────────────────────────────
// Axis-Aligned Bounding Box (AABB) collision detection.
// Stub for C1.

struct AABB {
    glm::vec3 min{0};
    glm::vec3 max{0};
};

namespace Collision {

    // Test if two AABBs overlap
    inline bool testAABB(const AABB& a, const AABB& b) {
        return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
               (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
               (a.min.z <= b.max.z && a.max.z >= b.min.z);
    }

    // TODO (C1): Add resolution helpers (push-back, slide)

} // namespace Collision

#endif // SUNDOWNDRIVE_GAMEPLAY_COLLISION_H
