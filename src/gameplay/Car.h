#ifndef SUNDOWNDRIVE_GAMEPLAY_CAR_H
#define SUNDOWNDRIVE_GAMEPLAY_CAR_H

#include "rendering/Model.h"
#include "rendering/Mesh.h"
#include "rendering/Texture.h"
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
    void setFallbackMesh(Mesh* mesh, Texture* texture);

    Transform&       getTransform()       { return transform_; }
    const Transform& getTransform() const { return transform_; }

    float getSpeed() const { return speed_; }
    float getYaw()   const { return transform_.rotation.y; }
    float getModelMinY() const { return hasModel_ ? model_.getBoundsMin().y : 0.0f; }
    float getModelMaxY() const { return hasModel_ ? model_.getBoundsMax().y : 1.0f; }
    float getModelMinX() const { return hasModel_ ? model_.getBoundsMin().x : -0.5f; }
    float getModelMaxX() const { return hasModel_ ? model_.getBoundsMax().x : 0.5f; }
    float getModelMinZ() const { return hasModel_ ? model_.getBoundsMin().z : -0.5f; }
    float getModelMaxZ() const { return hasModel_ ? model_.getBoundsMax().z : 0.5f; }

private:
    Model     model_;
    bool      hasModel_ = false;
    Mesh*     fallbackMesh_ = nullptr;
    Texture*  fallbackTexture_ = nullptr;
    Transform transform_;
    float     speed_ = 0.0f;
    float     steerAngle_ = 0.0f;
    float     moveSpeed_ = 8.0f;
    float     turnSpeed_ = 120.0f;
};

#endif // SUNDOWNDRIVE_GAMEPLAY_CAR_H
