#ifndef SUNDOWNDRIVE_CORE_CAMERA_H
#define SUNDOWNDRIVE_CORE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ── Camera ───────────────────────────────────────────────────────────────────
// Supports two modes:
//   FREE  – WASD + mouse look (for debugging / P3 requirement)
//   THIRD_PERSON – follows a target position at a fixed offset (for driving)
class Camera {
public:
    enum class Mode { FREE, THIRD_PERSON };

    Camera();

    // Call each frame to rebuild the view matrix
    void update(float deltaTime);

    // Getters
    glm::mat4 getViewMatrix()       const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    glm::vec3 getPosition()         const { return position_; }

    // Free-camera controls (called from Input callbacks)
    void processKeyboard(int direction, float deltaTime);   // 0=fwd 1=back 2=left 3=right
    void processMouse(float xOffset, float yOffset);

    // Third-person target
    void setTarget(const glm::vec3& targetPos, float targetYaw);
    void setMode(Mode mode) { mode_ = mode; }
    Mode getMode() const   { return mode_; }

private:
    Mode      mode_ = Mode::FREE;
    glm::vec3 position_ {0.0f, 3.0f, 8.0f};
    float     yaw_   = -90.0f;
    float     pitch_ =  -15.0f;
    float     speed_ =  8.0f;
    float     sensitivity_ = 0.15f;
    float     fov_   = 45.0f;

    // Third-person offset behind & above the target
    glm::vec3 followOffset_ {0.0f, 3.0f, 7.0f};

    glm::vec3 front_, right_, up_;
    void updateVectors();
};

#endif // SUNDOWNDRIVE_CORE_CAMERA_H
