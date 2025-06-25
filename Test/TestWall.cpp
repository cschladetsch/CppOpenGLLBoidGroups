#include "Wall.h"
#include <glm/glm.hpp>
#include <gtest/gtest.h>
#include <vector>

class WallTest : public ::testing::Test {
protected:
  void SetUp() override {
    wall = std::make_unique<Wall>(glm::vec3(0.0f, 1.0f, 0.0f),
                                  glm::vec3(10.0f, 2.0f, 5.0f));
  }

  std::unique_ptr<Wall> wall;
};

TEST_F(WallTest, PositionIsCorrect) {
  EXPECT_EQ(wall->GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
}

TEST_F(WallTest, SizeIsCorrect) {
  EXPECT_EQ(wall->GetSize(), glm::vec3(10.0f, 2.0f, 5.0f));
}

TEST_F(WallTest, ModelMatrixIncludesTranslation) {
  glm::mat4 model = wall->GetModelMatrix();
  glm::vec3 translation = glm::vec3(model[3]);

  EXPECT_FLOAT_EQ(translation.x, 0.0f);
  EXPECT_FLOAT_EQ(translation.y, 1.0f);
  EXPECT_FLOAT_EQ(translation.z, 0.0f);
}

TEST_F(WallTest, ModelMatrixIncludesScale) {
  glm::mat4 model = wall->GetModelMatrix();

  // Extract scale from model matrix
  glm::vec3 scale;
  scale.x = glm::length(glm::vec3(model[0]));
  scale.y = glm::length(glm::vec3(model[1]));
  scale.z = glm::length(glm::vec3(model[2]));

  EXPECT_FLOAT_EQ(scale.x, 10.0f);
  EXPECT_FLOAT_EQ(scale.y, 2.0f);
  EXPECT_FLOAT_EQ(scale.z, 5.0f);
}

TEST_F(WallTest, GenerateMeshCreatesValidData) {
  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  wall->GenerateMesh(vertices, indices);

  // Box has 24 vertices (4 per face, 6 faces)
  EXPECT_EQ(vertices.size(), 24 * 6); // 6 floats per vertex (position + normal)

  // Box has 36 indices (2 triangles per face, 3 indices per triangle, 6 faces)
  EXPECT_EQ(indices.size(), 36);
}

TEST_F(WallTest, GeneratedIndicesAreValid) {
  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  wall->GenerateMesh(vertices, indices);

  // All indices should be within vertex range
  size_t maxVertexIndex = vertices.size() / 6 - 1;
  for (unsigned int index : indices) {
    EXPECT_LE(index, maxVertexIndex);
  }
}