#ifndef SUNDOWNDRIVE_GAMEPLAY_TRAFFICCAR_H
#define SUNDOWNDRIVE_GAMEPLAY_TRAFFICCAR_H

#include "gameplay/Car.h"
#include <vector>
#include <glm/glm.hpp>

// ── TrafficCar ───────────────────────────────────────────────────────────────
// AI-controlled car that follows the road path or moves randomly.
// Stub for C2.
class TrafficCar {
public:
    enum class MovementType { PATH_FOLLOW, RANDOM };

    TrafficCar() = default;

    bool init(const std::string& modelPath, MovementType type);
    void update(float deltaTime);
    void draw(Shader& shader);

    Transform& getTransform() { return transform_; }

private:
    Model        model_;
    Transform    transform_;
    MovementType moveType_ = MovementType::PATH_FOLLOW;
    float        speed_ = 5.0f;
    int          currentPathIndex_ = 0;
    // const std::vector<glm::vec3>* pathPoints_ = nullptr;
};

#endif // SUNDOWNDRIVE_GAMEPLAY_TRAFFICCAR_H
