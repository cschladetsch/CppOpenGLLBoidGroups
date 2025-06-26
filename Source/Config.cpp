#include "Config.h"
#include <fstream>
#include <iostream>

// Helper functions for glm::vec3 JSON conversion
namespace nlohmann {
    template <>
    struct adl_serializer<glm::vec3> {
        static void to_json(json& j, const glm::vec3& v) {
            j = json::array({v.x, v.y, v.z});
        }

        static void from_json(const json& j, glm::vec3& v) {
            v.x = j[0]; 
            v.y = j[1]; 
            v.z = j[2];
        }
    };
}

Config Config::Load(const std::string& filename) {
    Config config;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Config file not found, using defaults\n";
            return config;
        }
        
        json j;
        file >> j;
        
        // Load values if they exist, keep defaults otherwise
        if (j.contains("width")) config.width = j["width"];
        if (j.contains("height")) config.height = j["height"];
        if (j.contains("particleCount")) config.particleCount = j["particleCount"];
        if (j.contains("gravity")) config.gravity = j["gravity"];
        if (j.contains("damping")) config.damping = j["damping"];
        if (j.contains("cameraPos")) config.cameraPos = j["cameraPos"];
        if (j.contains("cameraTarget")) config.cameraTarget = j["cameraTarget"];
        
        std::cout << "Loaded config: " << config.particleCount << " particles\n";
    }
    catch (const std::exception& e) {
        std::cout << "Config error: " << e.what() << ", using defaults\n";
    }
    
    return config;
}

void Config::Save(const std::string& filename) const {
    try {
        json j = {
            {"width", width},
            {"height", height},
            {"particleCount", particleCount},
            {"gravity", gravity},
            {"damping", damping},
            {"cameraPos", cameraPos},
            {"cameraTarget", cameraTarget}
        };
        
        std::ofstream file(filename);
        file << j.dump(2);
        std::cout << "Config saved to " << filename << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "Failed to save config: " << e.what() << "\n";
    }
}
