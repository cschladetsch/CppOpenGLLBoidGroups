#include "Wall.h"
#include <glm/gtc/matrix_transform.hpp>

Wall::Wall(const glm::vec3& position, const glm::vec3& size)
    : position(position)
    , size(size) {
}

glm::mat4 Wall::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);
    return model;
}

void Wall::GenerateMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices) const {
    float halfX = 0.5f;
    float halfY = 0.5f;
    float halfZ = 0.5f;
    
    vertices = {
        -halfX, -halfY, -halfZ,  0.0f,  0.0f, -1.0f,
         halfX, -halfY, -halfZ,  0.0f,  0.0f, -1.0f,
         halfX,  halfY, -halfZ,  0.0f,  0.0f, -1.0f,
        -halfX,  halfY, -halfZ,  0.0f,  0.0f, -1.0f,
        
        -halfX, -halfY,  halfZ,  0.0f,  0.0f,  1.0f,
         halfX, -halfY,  halfZ,  0.0f,  0.0f,  1.0f,
         halfX,  halfY,  halfZ,  0.0f,  0.0f,  1.0f,
        -halfX,  halfY,  halfZ,  0.0f,  0.0f,  1.0f,
        
        -halfX,  halfY,  halfZ, -1.0f,  0.0f,  0.0f,
        -halfX,  halfY, -halfZ, -1.0f,  0.0f,  0.0f,
        -halfX, -halfY, -halfZ, -1.0f,  0.0f,  0.0f,
        -halfX, -halfY,  halfZ, -1.0f,  0.0f,  0.0f,
        
         halfX,  halfY,  halfZ,  1.0f,  0.0f,  0.0f,
         halfX,  halfY, -halfZ,  1.0f,  0.0f,  0.0f,
         halfX, -halfY, -halfZ,  1.0f,  0.0f,  0.0f,
         halfX, -halfY,  halfZ,  1.0f,  0.0f,  0.0f,
        
        -halfX, -halfY, -halfZ,  0.0f, -1.0f,  0.0f,
         halfX, -halfY, -halfZ,  0.0f, -1.0f,  0.0f,
         halfX, -halfY,  halfZ,  0.0f, -1.0f,  0.0f,
        -halfX, -halfY,  halfZ,  0.0f, -1.0f,  0.0f,
        
        -halfX,  halfY, -halfZ,  0.0f,  1.0f,  0.0f,
         halfX,  halfY, -halfZ,  0.0f,  1.0f,  0.0f,
         halfX,  halfY,  halfZ,  0.0f,  1.0f,  0.0f,
        -halfX,  halfY,  halfZ,  0.0f,  1.0f,  0.0f,
    };
    
    indices = {
        0,  1,  2,  2,  3,  0,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20,
    };
}