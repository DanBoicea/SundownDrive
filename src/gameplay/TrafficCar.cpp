#include "gameplay/TrafficCar.h"

bool TrafficCar::init(const std::string& modelPath, MovementType type) {
    moveType_ = type;
    if (!modelPath.empty()) {
        model_.load(modelPath);
    }
    return true;
}

void TrafficCar::update(float /*deltaTime*/) {
    // TODO (C2): Implement path-following and random movement
}

void TrafficCar::draw(Shader& shader) {
    shader.use();
    shader.setMat4("model", transform_.getModelMatrix());
    model_.draw();
}
