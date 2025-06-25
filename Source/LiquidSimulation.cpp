#include "LiquidSimulation.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <GLFW/glfw3.h>

LiquidSimulation::LiquidSimulation(float width, float height)
    : width(width)
    , height(height)
    , gravity(-0.5f)  // Gentle gravity for slow movement
    , pressureConstant(2000.0f)
    , viscosityConstant(50.0f)
    , restDensity(1000.0f)
    , smoothingRadius(2.0f)  // Smaller for smaller blobs
    , damping(0.99f)
    , rng(std::random_device{}())
    , colorDist(0.3f, 1.0f)
    , positionDist(-width * 0.4f, width * 0.4f)
    , unitDist(0.0f, 1.0f)
    , percentDist(0, 99)
    , timeSinceLastSpawn(0.0f)
    , globalTime(0.0f) {
    
    InitializeWalls();
    InitializeParticles();
    
    // Initialize group centroids
    const int numGroups = 6;
    groupCentroids.clear();
    std::vector<glm::vec3> groupColors = {
        glm::vec3(0.2f, 0.6f, 1.0f),  // Blue
        glm::vec3(1.0f, 0.3f, 0.5f),  // Pink/Red
        glm::vec3(0.3f, 1.0f, 0.6f),  // Mint Green
        glm::vec3(1.0f, 0.7f, 0.2f),  // Orange/Yellow
        glm::vec3(0.8f, 0.3f, 1.0f),  // Purple
        glm::vec3(0.3f, 1.0f, 1.0f)   // Cyan
    };
    
    for (int i = 0; i < numGroups; ++i) {
        GroupCentroid centroid;
        float angle = (i / static_cast<float>(numGroups)) * 2.0f * M_PI;
        centroid.position = glm::vec3(cos(angle) * 15.0f, 2.0f, sin(angle) * 10.0f);
        centroid.velocity = glm::vec3(0.0f);
        centroid.color = groupColors[i];
        centroid.phase = static_cast<float>(i) * M_PI / 3.0f;
        groupCentroids.push_back(centroid);
    }
}

void LiquidSimulation::InitializeParticles() {
    const int numGroups = 6; // More color variety
    
    // Create distinct color palettes for each group
    std::vector<glm::vec3> groupColors = {
        glm::vec3(0.2f, 0.6f, 1.0f),  // Blue
        glm::vec3(1.0f, 0.3f, 0.5f),  // Pink/Red
        glm::vec3(0.3f, 1.0f, 0.6f),  // Mint Green
        glm::vec3(1.0f, 0.7f, 0.2f),  // Orange/Yellow
        glm::vec3(0.8f, 0.3f, 1.0f),  // Purple
        glm::vec3(0.3f, 1.0f, 1.0f)   // Cyan
    };
    
    for (int g = 0; g < numGroups; ++g) {
        glm::vec3 groupColor = groupColors[g];
        
        // Position groups in a rectangle pattern
        float angle = (g / static_cast<float>(numGroups)) * 2.0f * M_PI;
        float radius = 15.0f;
        glm::vec3 groupCenter(
            cos(angle) * radius,
            1.5f,  // Keep in shallow space
            sin(angle) * radius * 0.6f  // More rectangular distribution
        );
        
        // Create different compound shapes for each group
        CreateCompoundShape(groupCenter, groupColor, g % 4);
    }
}

void LiquidSimulation::InitializeWalls() {
    float wallHeight = 5.0f;  // Shallow box
    float halfWidth = 30.0f;  // Wide rectangle
    float halfDepth = 20.0f;  // Less deep than wide
    
    // Front and back walls
    walls.emplace_back(glm::vec3(0.0f, wallHeight * 0.5f, -halfDepth), glm::vec3(halfWidth * 2, wallHeight, 1.0f));
    walls.emplace_back(glm::vec3(0.0f, wallHeight * 0.5f, halfDepth), glm::vec3(halfWidth * 2, wallHeight, 1.0f));
    
    // Left and right walls
    walls.emplace_back(glm::vec3(-halfWidth, wallHeight * 0.5f, 0.0f), glm::vec3(1.0f, wallHeight, halfDepth * 2));
    walls.emplace_back(glm::vec3(halfWidth, wallHeight * 0.5f, 0.0f), glm::vec3(1.0f, wallHeight, halfDepth * 2));
    
    // Floor
    walls.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(halfWidth * 2, 0.1f, halfDepth * 2));
    
    // Ceiling
    walls.emplace_back(glm::vec3(0.0f, wallHeight, 0.0f), glm::vec3(halfWidth * 2, 0.1f, halfDepth * 2));
}

void LiquidSimulation::AddParticle(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& color) {
    LiquidParticle particle;
    particle.position = position;
    particle.velocity = velocity;
    particle.color = color;
    particle.targetColor = color;
    particle.baseRadius = 0.3f + unitDist(rng) * 0.9f; // 80% smaller (was 1.5-6.0, now 0.3-1.2)
    particle.radius = particle.baseRadius;
    particle.mass = 0.2f + unitDist(rng) * 0.8f; // Varied masses
    particle.colorTransitionSpeed = 2.0f + unitDist(rng) * 2.0f;
    particle.wavePhase = 0.0f;
    particle.waveAmplitude = 0.0f;
    particle.waveDecay = 0.85f + unitDist(rng) * 0.1f;
    particles.push_back(particle);
}

void LiquidSimulation::CreateCompoundShape(const glm::vec3& center, const glm::vec3& color, int shapeType) {
    switch (shapeType) {
    case 0: // Line shape
        {
            int numSpheres = 8;
            float spacing = 0.8f;  // 80% smaller spacing
            for (int i = 0; i < numSpheres; ++i) {
                float t = (i - numSpheres/2.0f) * spacing;
                glm::vec3 pos = center + glm::vec3(t, 0, 0);
                glm::vec3 vel(0.0f);
                AddParticle(pos, vel, color);
            }
        }
        break;
        
    case 1: // Triangle shape
        {
            int layers = 4;
            for (int layer = 0; layer < layers; ++layer) {
                for (int i = 0; i <= layer; ++i) {
                    float x = (i - layer/2.0f) * 0.7f;  // 80% smaller
                    float z = layer * 0.6f;  // 80% smaller
                    glm::vec3 pos = center + glm::vec3(x, 0, z);
                    glm::vec3 vel(0.0f);
                    AddParticle(pos, vel, color);
                }
            }
        }
        break;
        
    case 2: // Ring shape
        {
            int numSpheres = 12;
            float ringRadius = 1.6f;  // 80% smaller
            for (int i = 0; i < numSpheres; ++i) {
                float angle = (i / static_cast<float>(numSpheres)) * 2.0f * M_PI;
                glm::vec3 pos = center + glm::vec3(
                    cos(angle) * ringRadius,
                    0,
                    sin(angle) * ringRadius
                );
                glm::vec3 vel(0.0f);
                AddParticle(pos, vel, color);
            }
        }
        break;
        
    case 3: // Cross shape
        {
            int armLength = 5;
            // Horizontal arm
            for (int i = -armLength; i <= armLength; ++i) {
                if (i != 0) { // Skip center to avoid duplicate
                    glm::vec3 pos = center + glm::vec3(i * 0.6f, 0, 0);  // 80% smaller
                    AddParticle(pos, glm::vec3(0.0f), color);
                }
            }
            // Vertical arm
            for (int i = -armLength; i <= armLength; ++i) {
                glm::vec3 pos = center + glm::vec3(0, 0, i * 0.6f);  // 80% smaller
                AddParticle(pos, glm::vec3(0.0f), color);
            }
        }
        break;
        
    default: // Cluster shape (default)
        {
            int numSpheres = 15;
            for (int i = 0; i < numSpheres; ++i) {
                float u = unitDist(rng);
                float v = unitDist(rng);
                float w = unitDist(rng);
                
                float r = 1.2f * pow(w, 0.33f);  // 80% smaller
                float theta = u * 2.0f * M_PI;
                float phi = acos(std::max(-1.0f, std::min(1.0f, 2.0f * v - 1.0f)));
                
                glm::vec3 offset(
                    r * sin(phi) * cos(theta),
                    r * abs(cos(phi)) * 0.3f,  // Flatter in Y
                    r * sin(phi) * sin(theta)
                );
                
                AddParticle(center + offset, glm::vec3(0.0f), color);
            }
        }
        break;
    }
}

void LiquidSimulation::Update(float deltaTime) {
    // Update global time
    globalTime += deltaTime;
    
    // Spawn new particles periodically (disabled to reduce jitter)
    // timeSinceLastSpawn += deltaTime;
    // if (timeSinceLastSpawn >= spawnInterval && particles.size() < maxParticles) {
    //     timeSinceLastSpawn = 0.0f;
    //     SpawnNewParticle();
    // }
    
    UpdateCentroids(deltaTime);
    ApplyForces(deltaTime);
    UpdatePositions(deltaTime);
    UpdateColors(deltaTime);
    UpdateWaves(deltaTime);
    ResolveCollisions();
    HandleWallCollisions();
}

void LiquidSimulation::UpdateCentroids(float deltaTime) {
    
    // Update each group centroid with complex movement
    for (size_t i = 0; i < groupCentroids.size(); ++i) {
        auto& centroid = groupCentroids[i];
        
        // Periodically trigger waves from group centers
        float waveTime = globalTime + centroid.phase;
        if (sin(waveTime * 2.0f) > 0.95f && percentDist(rng) < 30) {
            // Find a particle near this centroid to start the wave
            for (size_t p = 0; p < particles.size(); ++p) {
                float colorDist = glm::length(particles[p].color - centroid.color);
                if (colorDist < 0.3f) {
                    float dist = glm::length(particles[p].position - centroid.position);
                    if (dist < 10.0f) {
                        PropagateWave(p, 0.8f);
                        break;
                    }
                }
            }
        }
        
        // More stochastic movement with Perlin-like noise
        float t = globalTime + centroid.phase;
        float noise1 = sin(t * 0.7f + i * 1.3f) + sin(t * 1.9f + i * 0.7f) * 0.5f;
        float noise2 = cos(t * 0.5f + i * 1.7f) + cos(t * 2.1f + i * 0.9f) * 0.5f;
        float noise3 = sin(t * 0.9f + i * 1.1f) + sin(t * 1.3f + i * 1.5f) * 0.5f;
        
        glm::vec3 targetVel(
            noise1 * 1.5f,  // Remove random jitter
            noise2 * 0.8f,
            noise3 * 1.5f
        );
        
        // Smooth velocity transition
        centroid.velocity += (targetVel - centroid.velocity) * deltaTime * 0.5f;
        centroid.position += centroid.velocity * deltaTime;
        
        // Keep centroids in bounds with soft boundaries
        float boundRadius = 25.0f;
        float dist = glm::length(glm::vec2(centroid.position.x, centroid.position.z));
        if (dist > boundRadius) {
            glm::vec2 toCenter = -glm::normalize(glm::vec2(centroid.position.x, centroid.position.z));
            centroid.velocity.x += toCenter.x * 1.0f * deltaTime;
            centroid.velocity.z += toCenter.y * 1.0f * deltaTime;
        }
        
        // Height bounds
        if (centroid.position.y < 1.0f) centroid.velocity.y += 2.0f * deltaTime;
        if (centroid.position.y > 3.0f) centroid.velocity.y -= 2.0f * deltaTime;
        
        // Morph centroid colors based on proximity to other centroids
        glm::vec3 avgColor = centroid.color;
        float influence = 1.0f;
        for (size_t j = 0; j < groupCentroids.size(); ++j) {
            if (i != j) {
                float d = glm::length(centroid.position - groupCentroids[j].position);
                if (d < 10.0f) {
                    float w = 1.0f - (d / 10.0f);
                    avgColor += groupCentroids[j].color * w;
                    influence += w;
                }
            }
        }
        centroid.color = avgColor / influence;
    }
}

void LiquidSimulation::UpdateColors(float deltaTime) {
    // Count particles of each color in local neighborhoods
    std::vector<std::vector<int>> colorCounts(particles.size());
    for (auto& counts : colorCounts) {
        counts.resize(groupCentroids.size(), 0);
    }
    
    // First pass: count nearby colors for each particle
    for (size_t i = 0; i < particles.size(); ++i) {
        // Count colors in neighborhood
        int totalNearby = 0;
        
        for (size_t j = 0; j < particles.size(); ++j) {
            if (i == j) continue;
            
            float dist = glm::length(particles[j].position - particles[i].position);
            if (dist < 2.0f) { // Smaller radius for smaller particles
                // Find which color group this particle belongs to
                int colorGroup = -1;
                float minColorDist = 999.0f;
                for (size_t c = 0; c < groupCentroids.size(); ++c) {
                    float cDist = glm::length(particles[j].color - groupCentroids[c].color);
                    if (cDist < minColorDist) {
                        minColorDist = cDist;
                        colorGroup = c;
                    }
                }
                
                if (colorGroup >= 0 && minColorDist < 0.5f) {
                    colorCounts[i][colorGroup]++;
                    totalNearby++;
                }
            }
        }
        
        // Takeover mechanic: if overwhelmed by another color, convert
        if (totalNearby > 3) { // Need at least 4 nearby particles
            int dominantGroup = -1;
            int maxCount = 0;
            
            // Find dominant color group
            for (size_t c = 0; c < groupCentroids.size(); ++c) {
                if (colorCounts[i][c] > maxCount) {
                    maxCount = colorCounts[i][c];
                    dominantGroup = c;
                }
            }
            
            // If overwhelmed (more than 70% of nearby particles are different color)
            float overwhelmRatio = static_cast<float>(maxCount) / totalNearby;
            if (dominantGroup >= 0 && overwhelmRatio > 0.7f) {
                // Check if this is a different color than current
                float currentColorDist = glm::length(particles[i].color - groupCentroids[dominantGroup].color);
                if (currentColorDist > 0.5f) {
                    // Takeover! Set target color to dominant group
                    particles[i].targetColor = groupCentroids[dominantGroup].color;
                    particles[i].colorTransitionSpeed = 5.0f; // Fast takeover
                }
            }
        }
        
        // Otherwise, try to maintain group cohesion
        else {
            // Find nearest centroid of same color
            int myGroup = -1;
            float minDist = 999.0f;
            for (size_t c = 0; c < groupCentroids.size(); ++c) {
                float colorDist = glm::length(particles[i].color - groupCentroids[c].color);
                if (colorDist < 0.3f) {
                    float dist = glm::length(particles[i].position - groupCentroids[c].position);
                    if (dist < minDist) {
                        minDist = dist;
                        myGroup = c;
                    }
                }
            }
            
            if (myGroup >= 0) {
                // Maintain group color
                particles[i].targetColor = groupCentroids[myGroup].color;
                particles[i].colorTransitionSpeed = 2.0f; // Normal speed
            }
        }
        
        // Apply color transition
        particles[i].color += (particles[i].targetColor - particles[i].color) * 
                              particles[i].colorTransitionSpeed * deltaTime;
    }
}

void LiquidSimulation::SpawnNewParticle() {
    // Randomly select a color group or create a blend
    std::vector<glm::vec3> groupColors = {
        glm::vec3(0.2f, 0.6f, 1.0f),  // Blue
        glm::vec3(1.0f, 0.3f, 0.5f),  // Pink/Red
        glm::vec3(0.3f, 1.0f, 0.6f),  // Mint Green
        glm::vec3(1.0f, 0.7f, 0.2f),  // Orange/Yellow
        glm::vec3(0.8f, 0.3f, 1.0f),  // Purple
        glm::vec3(0.3f, 1.0f, 1.0f)   // Cyan
    };
    
    // Sometimes spawn with a blended color
    glm::vec3 color;
    if (percentDist(rng) < 20) { // 20% chance of blended color
        std::uniform_int_distribution<size_t> groupDist(0, groupColors.size() - 1);
        size_t g1 = groupDist(rng);
        size_t g2 = groupDist(rng);
        float blend = unitDist(rng);
        color = groupColors[g1] * blend + groupColors[g2] * (1.0f - blend);
    } else {
        std::uniform_int_distribution<size_t> groupDist(0, groupColors.size() - 1);
        color = groupColors[groupDist(rng)];
    }
    
    // Find average position of this color group
    glm::vec3 avgPos(0.0f);
    int count = 0;
    for (const auto& p : particles) {
        if (glm::length(p.color - color) < 0.1f) {
            avgPos += p.position;
            count++;
        }
    }
    
    if (count > 0) {
        avgPos /= static_cast<float>(count);
        // Spawn near the group center with some random offset
        glm::vec3 offset(
            (unitDist(rng) - 0.5f) * 5.0f,
            3.5f,  // Spawn from above in shallow space
            (unitDist(rng) - 0.5f) * 5.0f
        );
        AddParticle(avgPos + offset, glm::vec3(0.0f), color);
    }
}

void LiquidSimulation::ApplyForces(float deltaTime) {
    static float time = 0.0f;
    time += deltaTime;
    
    for (size_t i = 0; i < particles.size(); ++i) {
        glm::vec3 force(0.0f);
        
        // Gentle gravity
        force.y += gravity * particles[i].mass;
        
        // Boid-like forces with dynamic centroid attraction
        glm::vec3 separation(0.0f), alignment(0.0f), cohesion(0.0f);
        float totalWeight = 0.0f;
        
        // Find nearest group centroid based on color
        int nearestCentroid = -1;
        float minColorDist = 999.0f;
        for (size_t c = 0; c < groupCentroids.size(); ++c) {
            float colorDist = glm::length(particles[i].color - groupCentroids[c].color);
            if (colorDist < minColorDist) {
                minColorDist = colorDist;
                nearestCentroid = c;
            }
        }
        
        // Attraction to moving centroid
        glm::vec3 centroidForce(0.0f);
        if (nearestCentroid >= 0) {
            glm::vec3 toCentroid = groupCentroids[nearestCentroid].position - particles[i].position;
            float dist = glm::length(toCentroid);
            if (dist > 0.1f) {
                // Stronger attraction when far, weaker when close
                float strength = std::min(dist / 20.0f, 1.0f) * (1.0f - minColorDist);
                centroidForce = (toCentroid / dist) * strength * 3.0f;
            }
        }
        
        for (size_t j = 0; j < particles.size(); ++j) {
            if (i == j) continue;
            
            glm::vec3 diff = particles[j].position - particles[i].position;
            float dist = glm::length(diff);
            
            // Check if same color group
            float colorDist = glm::length(particles[i].color - particles[j].color);
            
            if (dist < 5.0f && dist > 0.001f) {  // Smaller neighborhood for smaller blobs
                glm::vec3 normalized = diff / dist;
                
                // Color similarity affects attraction (0 = different, 1 = same)
                float colorSimilarity = 1.0f - (colorDist / 3.0f);
                colorSimilarity = std::max(0.0f, colorSimilarity);
                
                // Mass affects influence
                float massInfluence = particles[j].mass / (particles[i].mass + particles[j].mass);
                
                // Separation scaled for smaller blobs
                float separationDist = particles[i].radius + particles[j].radius + 0.2f;
                if (dist < separationDist) {
                    separation -= normalized * (separationDist - dist) * 5.0f * (2.0f - colorSimilarity);
                }
                
                // 3D Alignment - influenced by color similarity and mass
                glm::vec3 velDiff = particles[j].velocity - particles[i].velocity;
                alignment += velDiff * colorSimilarity * massInfluence * 0.5f;
                
                // 3D Cohesion - stronger for similar colors, with vertical component
                glm::vec3 posDiff = particles[j].position - particles[i].position;
                cohesion += posDiff * colorSimilarity * 0.3f;
                totalWeight += colorSimilarity;
            }
        }
        
        // Apply boid forces with proper 3D movement
        if (totalWeight > 0.1f) {
            alignment = alignment / totalWeight;
            cohesion = cohesion / totalWeight;
        }
        
        force += separation * 20.0f;  // Gentler forces for slower movement
        force += alignment * 10.0f;
        force += cohesion * 5.0f;
        force += centroidForce * 1.0f; // Gentle centroid following
        
        // Add 3D exploration force
        force += glm::vec3(
            (unitDist(rng) - 0.5f) * 0.5f,
            (unitDist(rng) - 0.5f) * 0.3f, // Vertical movement
            (unitDist(rng) - 0.5f) * 0.5f
        );
        
        // Trigger waves when groups merge
        if (totalWeight > 2.0f && percentDist(rng) < 5) { // 5% chance when near many particles
            PropagateWave(i, 0.5f);
        }
        
        // Add small pressure force for fluid behavior
        force += CalculatePressureForce(i) * 0.3f;
        
        particles[i].velocity += force * deltaTime / particles[i].mass;
        particles[i].velocity *= damping;
        
        // Lower velocity limit for slower movement
        float speed = glm::length(particles[i].velocity);
        if (speed > 5.0f) {
            particles[i].velocity = (particles[i].velocity / speed) * 5.0f;
        }
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
                    
                    // Trigger wave on collision
                    float collisionIntensity = std::min(1.0f, velAlongNormal * 0.1f);
                    PropagateWave(i, collisionIntensity);
                    PropagateWave(j, collisionIntensity * 0.8f);
                }
            }
        }
    }
}

void LiquidSimulation::HandleWallCollisions() {
    for (auto& particle : particles) {
        // Shallow rectangular boundaries
        float halfWidth = 30.0f;  // Wide
        float halfDepth = 20.0f;  // Less deep
        float maxHeight = 5.0f;   // Very shallow
        
        // X boundaries with reduced bounce
        if (particle.position.x - particle.radius < -halfWidth) {
            particle.position.x = -halfWidth + particle.radius;
            particle.velocity.x = -particle.velocity.x * 0.3f;
        }
        if (particle.position.x + particle.radius > halfWidth) {
            particle.position.x = halfWidth - particle.radius;
            particle.velocity.x = -particle.velocity.x * 0.3f;
        }
        
        // Z boundaries with reduced bounce
        if (particle.position.z - particle.radius < -halfDepth) {
            particle.position.z = -halfDepth + particle.radius;
            particle.velocity.z = -particle.velocity.z * 0.3f;
        }
        if (particle.position.z + particle.radius > halfDepth) {
            particle.position.z = halfDepth - particle.radius;
            particle.velocity.z = -particle.velocity.z * 0.3f;
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

void LiquidSimulation::UpdateWaves(float deltaTime) {
    // Update wave properties for each particle
    for (auto& particle : particles) {
        // Update wave phase
        particle.wavePhase += deltaTime * 2.0f; // Slower wave speed
        
        // Decay wave amplitude
        particle.waveAmplitude *= (1.0f - deltaTime * (1.0f - particle.waveDecay));
        
        // Keep radius constant - no size changes
        particle.radius = particle.baseRadius;
        
        // Apply wave motion to particle position for group movement
        float waveEffect = sin(particle.wavePhase) * particle.waveAmplitude;
        
        // Create wave-like group movements
        glm::vec3 waveForce(0.0f);
        waveForce.x = cos(particle.wavePhase * 1.3f) * waveEffect * 2.0f;
        waveForce.y = sin(particle.wavePhase * 2.1f) * waveEffect * 1.0f;
        waveForce.z = sin(particle.wavePhase * 0.7f) * waveEffect * 2.0f;
        
        // Apply wave force as velocity change
        particle.velocity += waveForce * deltaTime;
    }
}

void LiquidSimulation::PropagateWave(size_t sourceIndex, float intensity) {
    if (sourceIndex >= particles.size()) return;
    
    const auto& source = particles[sourceIndex];
    
    // Propagate wave to nearby particles of the same color group
    for (size_t i = 0; i < particles.size(); ++i) {
        if (i == sourceIndex) continue;
        
        float dist = glm::length(particles[i].position - source.position);
        float maxDist = 20.0f; // Wave propagation distance
        
        if (dist < maxDist && dist > 0.001f) {
            // Color similarity affects wave propagation
            float colorDist = glm::length(particles[i].color - source.color);
            float colorSimilarity = std::max(0.0f, 1.0f - colorDist);
            
            // Calculate wave intensity based on distance and color
            float falloff = 1.0f - (dist / maxDist);
            falloff = falloff * falloff * colorSimilarity; // Quadratic falloff with color weighting
            
            // Add wave energy with phase delay based on distance
            float phaseDelay = dist * 0.3f;
            particles[i].waveAmplitude = std::max(particles[i].waveAmplitude, 
                                                 intensity * falloff);
            
            // Synchronize phase for group movement
            if (colorSimilarity > 0.8f) {
                particles[i].wavePhase = source.wavePhase - phaseDelay;
            }
        }
    }
}