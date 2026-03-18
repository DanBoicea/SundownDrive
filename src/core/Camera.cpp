#include "core/Camera.h"
#include "utils/Constants.h"

Camera::Camera() {
    updateVectors();
}

void Camera::update(float /*deltaTime*/) {
    // In THIRD_PERSON mode the view is recomputed via setTarget().
    // In FREE mode nothing extra needed — vectors are updated on input.
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov_),
                            aspectRatio,
                            Constants::CameraNear,
                            Constants::CameraFar);
}

void Camera::processKeyboard(int direction, float deltaTime) {
    float velocity = speed_ * deltaTime;
    if (direction == 0) position_ += front_ * velocity;  // forward
    if (direction == 1) position_ -= front_ * velocity;  // backward
    if (direction == 2) position_ -= right_ * velocity;  // left
    if (direction == 3) position_ += right_ * velocity;  // right
}

void Camera::processMouse(float xOffset, float yOffset) {
    yaw_   += xOffset * sensitivity_;
    pitch_ += yOffset * sensitivity_;
    if (pitch_ >  89.0f) pitch_ =  89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;
    updateVectors();
}

void Camera::setTarget(const glm::vec3& targetPos, float targetYaw) {
    // Place camera behind the car using the car's yaw angle
    float rad = glm::radians(targetYaw);
    glm::vec3 offset;
    offset.x = -sin(rad) * followOffset_.z;
    offset.y =  followOffset_.y;
    offset.z = -cos(rad) * followOffset_.z;

    position_ = targetPos + offset;
    front_    = glm::normalize(targetPos - position_);
    right_    = glm::normalize(glm::cross(front_, glm::vec3(0, 1, 0)));
    up_       = glm::normalize(glm::cross(right_, front_));
}

void Camera::updateVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    f.y = sin(glm::radians(pitch_));
    f.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(f);
    right_ = glm::normalize(glm::cross(front_, glm::vec3(0.0f, 1.0f, 0.0f)));
    up_    = glm::normalize(glm::cross(right_, front_));
}
