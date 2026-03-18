#include "gameplay/Car.h"
#include <iostream>

bool Car::init(const std::string& modelPath) {
    // TODO (C1): Load car .obj model
    if (!modelPath.empty()) {
        if (!model_.load(modelPath)) {
            std::cerr << "[Car] Model load failed: " << modelPath << std::endl;
        }
    }
    transform_.position = glm::vec3(0.0f, 0.0f, 0.0f);
    return true;
}

void Car::update(float /*deltaTime*/) {
    // TODO (C1): Process input for acceleration, braking, steering
    //   - Read arrow keys / WASD from Input
    //   - Apply acceleration / friction to speed_
    //   - Rotate transform_.rotation.y by steerAngle_
    //   - Move transform_.position forward along facing direction
}

void Car::draw(Shader& shader) {
    shader.use();
    shader.setMat4("model", transform_.getModelMatrix());
    model_.draw();
}
