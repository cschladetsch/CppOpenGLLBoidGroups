#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera(const glm::vec3 &position);

  void SetTopDownView();
  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix(float aspectRatio) const;

private:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;

  float fov;
  float nearPlane;
  float farPlane;
};