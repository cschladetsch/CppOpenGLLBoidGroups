#pragma once
#include "Wall.h"
#include <boost/container/static_vector.hpp>
#include <glm/glm.hpp>
#include <random>
#include <vector>

struct LiquidParticle {
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 color;
  glm::vec3 targetColor;  // For smooth color transitions
  float radius;
  float baseRadius;       // Original radius before wave effects
  float mass;
  float colorTransitionSpeed;
  float wavePhase;        // Phase for wave propagation
  float waveAmplitude;    // Current wave amplitude
  float waveDecay;        // How fast the wave decays
};

class LiquidSimulation {
public:
  LiquidSimulation(float width, float height);

  void Update(float deltaTime);
  void AddParticle(const glm::vec3 &position, const glm::vec3 &velocity,
                   const glm::vec3 &color);

  const std::vector<LiquidParticle> &GetParticles() const { return particles; }
  const std::vector<Wall> &GetWalls() const { return walls; }
  size_t GetParticleCount() const { return particles.size(); }
  void SetGravity(const glm::vec3& g) { gravity = g.y; }
  void SetDamping(float d) { damping = d; }

private:
  void InitializeParticles();
  void InitializeWalls();
  void CreateCompoundShape(const glm::vec3& center, const glm::vec3& color, int shapeType);
  void ApplyForces(float deltaTime);
  void UpdatePositions(float deltaTime);
  void UpdateColors(float deltaTime);
  void UpdateCentroids(float deltaTime);
  void UpdateWaves(float deltaTime);
  void PropagateWave(size_t sourceIndex, float intensity);
  void ResolveCollisions();
  void HandleWallCollisions();
  void SpawnNewParticle();
  glm::vec3 CalculatePressureForce(size_t particleIndex);
  glm::vec3 CalculateViscosityForce(size_t particleIndex);

  std::vector<LiquidParticle> particles;
  std::vector<Wall> walls;
  std::vector<size_t> neighbors;
  
  // Group centroid tracking
  struct GroupCentroid {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float phase; // For movement patterns
  };
  std::vector<GroupCentroid> groupCentroids;

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
  std::uniform_real_distribution<float> unitDist;  // 0.0 to 1.0
  std::uniform_int_distribution<int> percentDist; // 0 to 99
  
  float timeSinceLastSpawn;
  const float spawnInterval = 0.05f; // More frequent spawning for better coverage
  const size_t maxParticles = 800; // More particles to fill the screen
  
  float globalTime; // Global time for synchronized animations
};