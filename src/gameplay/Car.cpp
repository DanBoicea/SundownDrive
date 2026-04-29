#include "gameplay/Car.h"
#include "core/Input.h"
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

bool Car::init(const std::string& modelPath) {
    if (!modelPath.empty()) {
        if (!model_.load(modelPath)) {
            std::cerr << "[Car] Model load failed: " << modelPath << std::endl;
        } else {
            hasModel_ = true;
        }
    }
    transform_.position = glm::vec3(0.0f, 0.0f, 0.0f);
    return true;
}

void Car::update(float deltaTime) {
    if (Input::isKeyDown('w')) speed_ = -1.0f;
    else if (Input::isKeyDown('s')) speed_ = 1.0f;
    else speed_ = 0.0f;

    float turn = 0.0f;
    if (Input::isKeyDown('a')) turn += 1.0f;
    if (Input::isKeyDown('d')) turn -= 1.0f;

    transform_.rotation.y += turn * turnSpeed_ * deltaTime;

    float yawRad = glm::radians(transform_.rotation.y);
    glm::vec3 forward(-std::sin(yawRad), 0.0f, -std::cos(yawRad));
    transform_.position += forward * (speed_ * moveSpeed_ * deltaTime);
}

void Car::draw(Shader& shader) {
    shader.use();
    shader.setMat4("model", transform_.getModelMatrix());
    if (hasModel_) {
        model_.draw(shader);
        return;
    }

    if (fallbackMesh_) {
        if (fallbackTexture_) {
            fallbackTexture_->bind(GL_TEXTURE0);
            shader.setInt("textureSampler", 0);
        }
        fallbackMesh_->draw();
    }
}

void Car::setFallbackMesh(Mesh* mesh, Texture* texture) {
    fallbackMesh_ = mesh;
    fallbackTexture_ = texture;
}
