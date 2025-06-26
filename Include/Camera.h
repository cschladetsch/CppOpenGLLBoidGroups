#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    explicit Camera(const glm::vec3& position = glm::vec3(0.0f, 5.0f, 10.0f));
    ~Camera() = default;

    // View controls
    void SetTopDownView();
    void SetPosition(const glm::vec3& position) { position_ = position; UpdateViewMatrix(); }
    void SetTarget(const glm::vec3& target) { target_ = target; UpdateViewMatrix(); }
    void SetUp(const glm::vec3& up) { up_ = up; UpdateViewMatrix(); }

    // Matrix getters
    glm::mat4 GetViewMatrix() const { return viewMatrix_; }
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;

    // Getters
    const glm::vec3& GetPosition() const { return position_; }
    const glm::vec3& GetTarget() const { return target_; }
    const glm::vec3& GetUp() const { return up_; }

    // Camera parameters - wider FOV for full window usage
    void SetFOV(float fov) { fov_ = fov; UpdateProjection(); }
    void SetNearPlane(float nearPlane) { nearPlane_ = nearPlane; UpdateProjection(); }
    void SetFarPlane(float farPlane) { farPlane_ = farPlane; UpdateProjection(); }
    void SetAspectRatio(float aspect) { aspectRatio_ = aspect; UpdateProjection(); }

private:
    glm::vec3 position_;
    glm::vec3 target_;
    glm::vec3 up_;
    
    glm::mat4 viewMatrix_;
    glm::mat4 projectionMatrix_;
    
    // Projection parameters - wider FOV to fill screen
    float fov_{60.0f};            // Increased from 45 to 60 degrees
    float aspectRatio_{16.0f/9.0f};
    float nearPlane_{0.1f};
    float farPlane_{200.0f};      // Increased far plane
    
    void UpdateViewMatrix();
    void UpdateProjection();
};
