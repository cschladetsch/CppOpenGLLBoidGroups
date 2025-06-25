#include <gtest/gtest.h>
#include "LiquidSimulation.h"
#include <glm/glm.hpp>

class LiquidSimulationTest : public ::testing::Test {
protected:
    void SetUp() override {
        simulation = std::make_unique<LiquidSimulation>(100.0f, 100.0f);
    }
    
    std::unique_ptr<LiquidSimulation> simulation;
};

TEST_F(LiquidSimulationTest, InitializationCreatesParticles) {
    EXPECT_GT(simulation->GetParticles().size(), 0);
}

TEST_F(LiquidSimulationTest, InitializationCreatesWalls) {
    EXPECT_EQ(simulation->GetWalls().size(), 4);
}

TEST_F(LiquidSimulationTest, AddParticleIncreasesCount) {
    size_t initialCount = simulation->GetParticles().size();
    simulation->AddParticle(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    EXPECT_EQ(simulation->GetParticles().size(), initialCount + 1);
}

TEST_F(LiquidSimulationTest, UpdateMaintainsParticleCount) {
    size_t initialCount = simulation->GetParticles().size();
    simulation->Update(0.016f); // ~60Hz
    EXPECT_EQ(simulation->GetParticles().size(), initialCount);
}

TEST_F(LiquidSimulationTest, ParticlesHaveValidColors) {
    for (const auto& particle : simulation->GetParticles()) {
        EXPECT_GE(particle.color.r, 0.0f);
        EXPECT_LE(particle.color.r, 1.0f);
        EXPECT_GE(particle.color.g, 0.0f);
        EXPECT_LE(particle.color.g, 1.0f);
        EXPECT_GE(particle.color.b, 0.0f);
        EXPECT_LE(particle.color.b, 1.0f);
    }
}

TEST_F(LiquidSimulationTest, ParticlesStayWithinBounds) {
    for (int i = 0; i < 100; ++i) {
        simulation->Update(0.016f);
    }
    
    float halfWidth = 50.0f;
    float halfHeight = 50.0f;
    
    for (const auto& particle : simulation->GetParticles()) {
        EXPECT_GE(particle.position.x, -halfWidth - particle.radius);
        EXPECT_LE(particle.position.x, halfWidth + particle.radius);
        EXPECT_GE(particle.position.z, -halfHeight - particle.radius);
        EXPECT_LE(particle.position.z, halfHeight + particle.radius);
        EXPECT_GE(particle.position.y, 0.0f);
    }
}