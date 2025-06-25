#include "Wall.h"

Wall::Wall(const glm::vec3& position, const glm::vec3& size)
    : position_(position), size_(size) {
}

void Wall::GenerateMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices) const {
    vertices.clear();
    indices.clear();
    
    // Generate a simple box mesh
    // 8 vertices of a box
    glm::vec3 halfSize = size_ * 0.5f;
    
    // Bottom face (y = -halfSize.y)
    vertices.insert(vertices.end(), {
        -halfSize.x, -halfSize.y, -halfSize.z,  // 0
         halfSize.x, -halfSize.y, -halfSize.z,  // 1
         halfSize.x, -halfSize.y,  halfSize.z,  // 2
        -halfSize.x, -halfSize.y,  halfSize.z   // 3
    });
    
    // Top face (y = halfSize.y)
    vertices.insert(vertices.end(), {
        -halfSize.x,  halfSize.y, -halfSize.z,  // 4
         halfSize.x,  halfSize.y, -halfSize.z,  // 5
         halfSize.x,  halfSize.y,  halfSize.z,  // 6
        -halfSize.x,  halfSize.y,  halfSize.z   // 7
    });
    
    // Generate indices for a box (12 triangles = 36 indices)
    indices = {
        // Bottom face
        0, 1, 2,  2, 3, 0,
        // Top face
        4, 7, 6,  6, 5, 4,
        // Front face
        0, 4, 5,  5, 1, 0,
        // Back face
        2, 6, 7,  7, 3, 2,
        // Left face
        0, 3, 7,  7, 4, 0,
        // Right face
        1, 5, 6,  6, 2, 1
    };
}

glm::mat4 Wall::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position_);
    model = glm::scale(model, size_);
    return model;
}
