#ifndef SUNDOWNDRIVE_UTILS_TRANSFORM_H
#define SUNDOWNDRIVE_UTILS_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ── Transform component: position, rotation (Euler degrees), scale ───────────
// Computes the model matrix on demand.  Every renderable object owns one.
struct Transform {
    glm::vec3 position {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation {0.0f, 0.0f, 0.0f};  // Euler angles in degrees (pitch, yaw, roll)
    glm::vec3 scale    {1.0f, 1.0f, 1.0f};

    // Build a 4x4 model matrix: T * Ry * Rx * Rz * S
    glm::mat4 getModelMatrix() const {
        glm::mat4 m(1.0f);
        m = glm::translate(m, position);
        m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // yaw
        m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // pitch
        m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // roll
        m = glm::scale(m, scale);
        return m;
    }
};

#endif // SUNDOWNDRIVE_UTILS_TRANSFORM_H
