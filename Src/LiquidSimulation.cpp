#include "LiquidSimulation.h"
#include <algorithm>
#include <cmath>

LiquidSimulation::LiquidSimulation(float width, float height)
    : width(width)
    , height(height)
    , gravity(-9.81f)
    , pressureConstant(1000.0f)
    , viscosityConstant(0.5f)
    , restDensity(1000.0f)
    , smoothingRadius(2.0f)
    , damping(0.995f)
    , rng(std::random_device{}())
    , colorDist(0.3f, 1.0f)
    , positionDist(-width * 0.4f, width * 0.4f) {
    
    InitializeWalls();
    InitializeParticles();
}

void LiquidSimulation::InitializeParticles() {
    const int numGroups = 5;
    const int particlesPerGroup = 50;
    
    for (int g = 0; g < numGroups; ++g) {
        glm::vec3 groupColor(
            colorDist(rng),
            colorDist(rng),
            colorDist(rng)
        );
        
        glm::vec3 groupCenter(
            positionDist(rng),
            0.5f,
            positionDist(rng)
        );
        
        for (int i = 0; i < particlesPerGroup; ++i) {
            float angle = (i / static_cast<float>(particlesPerGroup)) * 2.0f * M_PI;
            float radius = (i % 5) * 0.5f + 1.0f;
            
            glm::vec3 offset(
                cos(angle) * radius,
                0.0f,
                sin(angle) * radius
            );
            
            AddParticle(groupCenter + offset, glm::vec3(0.0f), groupColor);
        }
    }
}

void LiquidSimulation::InitializeWalls() {
    float wallHeight = 5.0f;
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    walls.emplace_back(glm::vec3(0.0f, 0.0f, -halfHeight), glm::vec3(width, wallHeight, 1.0f));
    walls.emplace_back(glm::vec3(0.0f, 0.0f, halfHeight), glm::vec3(width, wallHeight, 1.0f));
    walls.emplace_back(glm::vec3(-halfWidth, 0.0f, 0.0f), glm::vec3(1.0f, wallHeight, height));
    walls.emplace_back(glm::vec3(halfWidth, 0.0f, 0.0f), glm::vec3(1.0f, wallHeight, height));
}

void LiquidSimulation::AddParticle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color) {
    LiquidParticle particle;
    particle.position = position;
    particle.velocity = velocity;
    particle.color = color;
    particle.radius = 0.5f;
    particle.mass = 1.0f;
    particles.push_back(particle);
}

void LiquidSimulation::Update(float deltaTime) {
    ApplyForces(deltaTime);
    UpdatePositions(deltaTime);
    ResolveCollisions();
    HandleWallCollisions();
}

void LiquidSimulation::ApplyForces(float deltaTime) {
    for (size_t i = 0; i < particles.size(); ++i) {
        glm::vec3 force(0.0f);
        
        force.y += gravity * particles[i].mass;
        
        force += CalculatePressureForce(i);
        force += CalculateViscosityForce(i);
        
        particles[i].velocity += force * deltaTime / particles[i].mass;
        particles[i].velocity *= damping;
    }
}

void LiquidSimulation::UpdatePositions(float deltaTime) {
    for (auto& particle : particles) {
        particle.position += particle.velocity * deltaTime;
    }
}

void LiquidSimulation::ResolveCollisions() {
    for (size_t i = 0; i < particles.size(); ++i) {
        for (size_t j = i + 1; j < particles.size(); ++j) {
            glm::vec3 diff = particles[i].position - particles[j].position;
            float distance = glm::length(diff);
            float minDistance = particles[i].radius + particles[j].radius;
            
            if (distance < minDistance && distance > 0.0001f) {
                glm::vec3 normal = diff / distance;
                float overlap = minDistance - distance;
                
                particles[i].position += normal * overlap * 0.5f;
                particles[j].position -= normal * overlap * 0.5f;
                
                glm::vec3 relVel = particles[i].velocity - particles[j].velocity;
                float velAlongNormal = glm::dot(relVel, normal);
                
                if (velAlongNormal > 0) {
                    float restitution = 0.3f;
                    float j = -(1 + restitution) * velAlongNormal;
                    j /= 1.0f / particles[i].mass + 1.0f / particles[j].mass;
                    
                    glm::vec3 impulse = j * normal;
                    particles[i].velocity += impulse / particles[i].mass;
                    particles[j].velocity -= impulse / particles[j].mass;
                }
            }
        }
    }
}

void LiquidSimulation::HandleWallCollisions() {
    for (auto& particle : particles) {
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        
        if (particle.position.x - particle.radius < -halfWidth) {
            particle.position.x = -halfWidth + particle.radius;
            particle.velocity.x = -particle.velocity.x * 0.5f;
        }
        if (particle.position.x + particle.radius > halfWidth) {
            particle.position.x = halfWidth - particle.radius;
            particle.velocity.x = -particle.velocity.x * 0.5f;
        }
        
        if (particle.position.z - particle.radius < -halfHeight) {
            particle.position.z = -halfHeight + particle.radius;
            particle.velocity.z = -particle.velocity.z * 0.5f;
        }
        if (particle.position.z + particle.radius > halfHeight) {
            particle.position.z = halfHeight - particle.radius;
            particle.velocity.z = -particle.velocity.z * 0.5f;
        }
        
        if (particle.position.y - particle.radius < 0.0f) {
            particle.position.y = particle.radius;
            particle.velocity.y = -particle.velocity.y * 0.3f;
        }
    }
}

glm::vec3 LiquidSimulation::CalculatePressureForce(size_t particleIndex) {
    glm::vec3 force(0.0f);
    neighbors.clear();
    
    for (size_t i = 0; i < particles.size(); ++i) {
        if (i != particleIndex) {
            float dist = glm::length(particles[i].position - particles[particleIndex].position);
            if (dist < smoothingRadius) {
                neighbors.push_back(i);
            }
        }
    }
    
    float density = 0.0f;
    for (size_t i : neighbors) {
        float dist = glm::length(particles[i].position - particles[particleIndex].position);
        float influence = 1.0f - (dist / smoothingRadius);
        density += particles[i].mass * influence * influence;
    }
    
    float pressure = pressureConstant * (density - restDensity);
    
    for (size_t i : neighbors) {
        glm::vec3 diff = particles[particleIndex].position - particles[i].position;
        float dist = glm::length(diff);
        if (dist > 0.0001f) {
            float influence = 1.0f - (dist / smoothingRadius);
            force += (diff / dist) * pressure * influence;
        }
    }
    
    return force;
}

glm::vec3 LiquidSimulation::CalculateViscosityForce(size_t particleIndex) {
    glm::vec3 force(0.0f);
    
    for (size_t i : neighbors) {
        glm::vec3 velDiff = particles[i].velocity - particles[particleIndex].velocity;
        float dist = glm::length(particles[i].position - particles[particleIndex].position);
        float influence = 1.0f - (dist / smoothingRadius);
        force += velDiff * viscosityConstant * influence;
    }
    
    return force;
}