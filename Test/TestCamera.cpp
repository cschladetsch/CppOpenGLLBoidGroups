#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gtest/gtest.h>

class CameraTest : public ::testing::Test {
protected:
  void SetUp() override {
    camera = std::make_unique<Camera>(glm::vec3(0.0f, 10.0f, 0.0f));
  }

  std::unique_ptr<Camera> camera;
};

TEST_F(CameraTest, InitialPositionIsCorrect) {
  glm::mat4 view = camera->GetViewMatrix();
  glm::vec3 position = glm::vec3(glm::inverse(view)[3]);
  EXPECT_NEAR(position.x, 0.0f, 0.0001f);
  EXPECT_NEAR(position.y, 10.0f, 0.0001f);
  EXPECT_NEAR(position.z, 0.0f, 0.0001f);
}

TEST_F(CameraTest, TopDownViewLooksDown) {
  camera->SetTopDownView();
  glm::mat4 view = camera->GetViewMatrix();

  // Extract forward vector (negative Z axis in view space)
  glm::vec3 forward = -glm::vec3(view[0][2], view[1][2], view[2][2]);

  // Should be looking down (negative Y)
  EXPECT_NEAR(forward.x, 0.0f, 0.001f);
  EXPECT_NEAR(forward.y, -1.0f, 0.001f);
  EXPECT_NEAR(forward.z, 0.0f, 0.001f);
}

TEST_F(CameraTest, ProjectionMatrixIsValid) {
  glm::mat4 proj = camera->GetProjectionMatrix(16.0f / 9.0f);

  // Check that it's not identity
  EXPECT_NE(proj, glm::mat4(1.0f));

  // Check perspective properties
  EXPECT_NE(proj[2][3], 0.0f); // Perspective division
  EXPECT_EQ(proj[3][3], 0.0f); // Perspective matrix property
}

TEST_F(CameraTest, AspectRatioAffectsProjection) {
  glm::mat4 proj1 = camera->GetProjectionMatrix(16.0f / 9.0f);
  glm::mat4 proj2 = camera->GetProjectionMatrix(4.0f / 3.0f);

  EXPECT_NE(proj1, proj2);
  EXPECT_NE(proj1[0][0], proj2[0][0]); // X scaling should differ
}