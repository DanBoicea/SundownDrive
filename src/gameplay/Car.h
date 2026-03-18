#ifndef SUNDOWNDRIVE_GAMEPLAY_CAR_H
#define SUNDOWNDRIVE_GAMEPLAY_CAR_H

#include "rendering/Model.h"
#include "utils/Transform.h"
#include "core/Shader.h"

// ── Car ──────────────────────────────────────────────────────────────────────
// Player-controlled vehicle. Reads input to accelerate, brake, and steer.
// Stub for C1 — will be fleshed out with physics and model loading.
class Car {
public:
    Car() = default;

    bool init(const std::string& modelPath);
    void update(float deltaTime);
    void draw(Shader& shader);

    Transform&       getTransform()       { return transform_; }
    const Transform& getTransform() const { return transform_; }

    float getSpeed() const { return speed_; }
    float getYaw()   const { return transform_.rotation.y; }

private:
    Model     model_;
    Transform transform_;
    float     speed_ = 0.0f;
    float     steerAngle_ = 0.0f;
};

#endif // SUNDOWNDRIVE_GAMEPLAY_CAR_H
