#ifndef SUNDOWNDRIVE_GAMEPLAY_COLLISION_H
#define SUNDOWNDRIVE_GAMEPLAY_COLLISION_H

#include <glm/glm.hpp>

// stub
struct AABB {
    glm::vec3 min{0};
    glm::vec3 max{0};
};

namespace Collision {
    inline bool testAABB(const AABB& a, const AABB& b) {
        return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
               (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
               (a.min.z <= b.max.z && a.max.z >= b.min.z);
    }

}

#endif
