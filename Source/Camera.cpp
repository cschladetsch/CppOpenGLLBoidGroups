#include "Camera.h"

Camera::Camera(const glm::vec3& position)
    : position(position)
    , front(glm::normalize(glm::vec3(0.0f, -30.0f, -80.0f)))
    , up(0.0f, 1.0f, 0.0f)
    , right(1.0f, 0.0f, 0.0f)
    , fov(75.0f) // Wider field of view
    , nearPlane(0.1f)
    , farPlane(200.0f) {
}

void Camera::SetTopDownView() {
    front = glm::vec3(0.0f, -1.0f, 0.0f);
    up = glm::vec3(0.0f, 0.0f, -1.0f);
    right = glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}