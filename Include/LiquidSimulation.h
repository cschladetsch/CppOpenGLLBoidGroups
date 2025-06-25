#pragma once
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <boost/container/static_vector.hpp>
#include "Wall.h"

struct LiquidParticle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float radius;
    float mass;
};

class LiquidSimulation {
public:
    LiquidSimulation(float width, float height);
    
    void Update(float deltaTime);
    void AddParticle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color);
    
    const std::vector<LiquidParticle>& GetParticles() const { return particles; }
    const std::vector<Wall>& GetWalls() const { return walls; }
    
private:
    void InitializeParticles();
    void InitializeWalls();
    void ApplyForces(float deltaTime);
    void UpdatePositions(float deltaTime);
    void ResolveCollisions();
    void HandleWallCollisions();
    glm::vec3 CalculatePressureForce(size_t particleIndex);
    glm::vec3 CalculateViscosityForce(size_t particleIndex);
    
    std::vector<LiquidParticle> particles;
    std::vector<Wall> walls;
    std::vector<size_t> neighbors;
    
    float width, height;
    float gravity;
    float pressureConstant;
    float viscosityConstant;
    float restDensity;
    float smoothingRadius;
    float damping;
    
    std::mt19937 rng;
    std::uniform_real_distribution<float> colorDist;
    std::uniform_real_distribution<float> positionDist;
};