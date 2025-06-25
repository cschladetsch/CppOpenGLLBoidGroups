#pragma once
#include <glm/glm.hpp>
#include <vector>

class Wall {
public:
    Wall(const glm::vec3& position, const glm::vec3& size);
    
    const glm::vec3& GetPosition() const { return position; }
    const glm::vec3& GetSize() const { return size; }
    glm::mat4 GetModelMatrix() const;
    
    void GenerateMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices) const;
    
private:
    glm::vec3 position;
    glm::vec3 size;
};