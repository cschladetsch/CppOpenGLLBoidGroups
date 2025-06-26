#pragma once

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <string>

using json = nlohmann::json;

struct Config {
    // Simulation - much larger area to fill entire screen
    float width = 120.0f;         // Much wider 
    float height = 80.0f;         // Much taller
    int particleCount = 25000;    
    
    // Physics
    float gravity = -12.0f;       
    float damping = 0.98f;        
    
    // Camera - positioned to see massive area and fill entire window
    glm::vec3 cameraPos = glm::vec3(60.0f, 40.0f, 100.0f);  // Much further back
    glm::vec3 cameraTarget = glm::vec3(60.0f, 40.0f, 0.0f); // Center of large area
    
    // Load from JSON file, fallback to defaults if missing
    static Config Load(const std::string& filename = "config.json");
    
    // Save to JSON file
    void Save(const std::string& filename = "config.json") const;
};
