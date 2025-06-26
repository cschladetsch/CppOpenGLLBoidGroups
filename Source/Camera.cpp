#include "Camera.h"

Camera::Camera(const glm::vec3& position)
    : position_(position), target_(0.0f, 0.0f, 0.0f), up_(0.0f, 1.0f, 0.0f) {
    UpdateViewMatrix();
    UpdateProjection();
}

void Camera::SetTopDownView() {
    position_ = glm::vec3(0.0f, 20.0f, 0.0f);
    target_ = glm::vec3(0.0f, 0.0f, 0.0f);
    up_ = glm::vec3(0.0f, 0.0f, -1.0f);
    UpdateViewMatrix();
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const {
    // Always recalculate with current aspect ratio for proper scaling
    return glm::perspective(glm::radians(fov_), aspectRatio, nearPlane_, farPlane_);
}

void Camera::UpdateViewMatrix() {
    viewMatrix_ = glm::lookAt(position_, target_, up_);
}

void Camera::UpdateProjection() {
    projectionMatrix_ = glm::perspective(glm::radians(fov_), aspectRatio_, nearPlane_, farPlane_);
}
