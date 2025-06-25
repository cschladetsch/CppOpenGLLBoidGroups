#include "LiquidSimulation.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

LiquidSimulation::LiquidSimulation(float width, float height)
    : width(width)
    , height(height)
    , gravity(-1.0f)
    , pressureConstant(3000.0f)
    , viscosityConstant(10.0f)
    , restDensity(1000.0f)
    , smoothingRadius(3.0f)
    , damping(0.99f)
    , rng(std::random_device{}())
    , colorDist(0.3f, 1.0f)
    , positionDist(-width * 0.4f, width * 0.4f) {
    
    InitializeWalls();
    InitializeParticles();
}

void LiquidSimulation::InitializeParticles() {
    const int numGroups = 4;
    const int particlesPerGroup = 200;
    
    // Create distinct color palettes for each group
    std::vector<glm::vec3> groupColors = {
        glm::vec3(0.3f, 0.7f, 1.0f),  // Blue
        glm::vec3(1.0f, 0.4f, 0.4f),  // Red
        glm::vec3(0.4f, 1.0f, 0.4f),  // Green
        glm::vec3(1.0f, 0.9f, 0.3f)   // Yellow
    };
    
    for (int g = 0; g < numGroups; ++g) {
        glm::vec3 groupColor = groupColors[g];
        
        // Position groups in different areas
        float angle = (g / static_cast<float>(numGroups)) * 2.0f * M_PI;
        float groupRadius = 15.0f;
        glm::vec3 groupCenter(
            cos(angle) * groupRadius,
            5.0f,  // Start higher up
            sin(angle) * groupRadius
        );
        
        // Create a tight blob of particles
        for (int i = 0; i < particlesPerGroup; ++i) {
            // Create a dense sphere of particles
            float u = static_cast<float>(rand()) / RAND_MAX;
            float v = static_cast<float>(rand()) / RAND_MAX;
            float w = static_cast<float>(rand()) / RAND_MAX;
            
            // Tighter initial distribution
            float r = 2.0f * pow(w, 0.33f);
            float theta = u * 2.0f * M_PI;
            float phi = acos(std::max(-1.0f, std::min(1.0f, 2.0f * v - 1.0f)));
            
            glm::vec3 offset(
                r * sin(phi) * cos(theta),
                r * abs(cos(phi)),  // Keep particles above ground
                r * sin(phi) * sin(theta)
            );
            
            // Add slight attraction toward group center
            glm::vec3 toCenter = -offset * 0.1f;
            glm::vec3 initialVel = toCenter + glm::vec3(
                (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f,
                0.0f,
                (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f
            );
            
            AddParticle(groupCenter + offset, initialVel, groupColor);
        }
    }
}

void LiquidSimulation::InitializeWalls() {
    float wallHeight = 30.0f;
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    // Front and back walls
    walls.emplace_back(glm::vec3(0.0f, wallHeight * 0.5f, -halfHeight), glm::vec3(width, wallHeight, 1.0f));
    walls.emplace_back(glm::vec3(0.0f, wallHeight * 0.5f, halfHeight), glm::vec3(width, wallHeight, 1.0f));
    
    // Left and right walls
    walls.emplace_back(glm::vec3(-halfWidth, wallHeight * 0.5f, 0.0f), glm::vec3(1.0f, wallHeight, height));
    walls.emplace_back(glm::vec3(halfWidth, wallHeight * 0.5f, 0.0f), glm::vec3(1.0f, wallHeight, height));
    
    // Floor
    walls.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(width, 0.1f, height));
    
    // Ceiling (transparent)
    walls.emplace_back(glm::vec3(0.0f, wallHeight, 0.0f), glm::vec3(width, 0.1f, height));
}

void LiquidSimulation::AddParticle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color) {
    LiquidParticle particle;
    particle.position = position;
    particle.velocity = velocity;
    particle.color = color;
    particle.radius = 0.8f;
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
            float distSq = glm::dot(diff, diff);
            float minDistance = particles[i].radius + particles[j].radius;
            float minDistSq = minDistance * minDistance;
            
            if (distSq < minDistSq && distSq > 0.0001f) {
                float distance = std::sqrt(distSq);
                glm::vec3 normal = diff / distance;
                float overlap = minDistance - distance;
                
                particles[i].position += normal * overlap * 0.5f;
                particles[j].position -= normal * overlap * 0.5f;
                
                glm::vec3 relVel = particles[i].velocity - particles[j].velocity;
                float velAlongNormal = glm::dot(relVel, normal);
                
                if (velAlongNormal > 0) {
                    float restitution = 0.1f;
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
        float maxHeight = 30.0f; // 3D box ceiling
        
        // X boundaries
        if (particle.position.x - particle.radius < -halfWidth) {
            particle.position.x = -halfWidth + particle.radius;
            particle.velocity.x = -particle.velocity.x * 0.7f;
        }
        if (particle.position.x + particle.radius > halfWidth) {
            particle.position.x = halfWidth - particle.radius;
            particle.velocity.x = -particle.velocity.x * 0.7f;
        }
        
        // Z boundaries
        if (particle.position.z - particle.radius < -halfHeight) {
            particle.position.z = -halfHeight + particle.radius;
            particle.velocity.z = -particle.velocity.z * 0.7f;
        }
        if (particle.position.z + particle.radius > halfHeight) {
            particle.position.z = halfHeight - particle.radius;
            particle.velocity.z = -particle.velocity.z * 0.7f;
        }
        
        // Y boundaries (floor and ceiling)
        if (particle.position.y - particle.radius < 0.0f) {
            particle.position.y = particle.radius;
            particle.velocity.y = -particle.velocity.y * 0.5f;
        }
        if (particle.position.y + particle.radius > maxHeight) {
            particle.position.y = maxHeight - particle.radius;
            particle.velocity.y = -particle.velocity.y * 0.5f;
        }
    }
}

glm::vec3 LiquidSimulation::CalculatePressureForce(size_t particleIndex) {
    glm::vec3 force(0.0f);
    neighbors.clear();
    
    // Find neighbors and check if they're from the same group (similar color)
    for (size_t i = 0; i < particles.size(); ++i) {
        if (i != particleIndex) {
            float dist = glm::length(particles[i].position - particles[particleIndex].position);
            if (dist < smoothingRadius) {
                neighbors.push_back(i);
            }
        }
    }
    
    float density = particles[particleIndex].mass; // Include self
    for (size_t i : neighbors) {
        float dist = glm::length(particles[i].position - particles[particleIndex].position);
        if (smoothingRadius > 0.0f) {
            float influence = std::max(0.0f, 1.0f - (dist / smoothingRadius));
            
            // Stronger influence if same color group
            glm::vec3 colorDiff = particles[i].color - particles[particleIndex].color;
            float colorSimilarity = std::max(0.1f, 1.0f - glm::length(colorDiff) * 0.3f);
            
            density += particles[i].mass * influence * influence * colorSimilarity;
        }
    }
    
    float pressure = pressureConstant * (density - restDensity);
    
    for (size_t i : neighbors) {
        glm::vec3 diff = particles[particleIndex].position - particles[i].position;
        float dist = glm::length(diff);
        if (dist > 0.0001f) {
            float influence = 1.0f - (dist / smoothingRadius);
            
            // Cohesion force for same color group
            glm::vec3 colorDiff = particles[i].color - particles[particleIndex].color;
            float colorSimilarity = 1.0f - glm::length(colorDiff) * 0.3f;
            
            force += (diff / dist) * pressure * influence * colorSimilarity;
        }
    }
    
    return force;
}

glm::vec3 LiquidSimulation::CalculateViscosityForce(size_t particleIndex) {
    glm::vec3 force(0.0f);
    
    for (size_t i : neighbors) {
        glm::vec3 velDiff = particles[i].velocity - particles[particleIndex].velocity;
        float dist = glm::length(particles[i].position - particles[particleIndex].position);
        if (dist > 0.0001f && smoothingRadius > 0.0f) {
            float influence = 1.0f - (dist / smoothingRadius);
            force += velDiff * viscosityConstant * influence;
        }
    }
    
    return force;
}