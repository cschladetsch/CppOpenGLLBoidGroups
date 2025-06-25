#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "Renderer.h"
#include "LiquidSimulation.h"
#include "Camera.h"

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --width <width>    Set window width (default: 1280)\n";
    std::cout << "  --height <height>  Set window height (default: 720)\n";
    std::cout << "  --help             Show this help message\n";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--width" && i + 1 < argc) {
            WINDOW_WIDTH = std::atoi(argv[++i]);
        } else if (arg == "--height" && i + 1 < argc) {
            WINDOW_HEIGHT = std::atoi(argv[++i]);
        } else if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Liquid Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Enable V-Sync for 60Hz
    glfwSwapInterval(1);
    
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    std::cout << "GLEW initialized successfully" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    
    auto camera = std::make_unique<Camera>(glm::vec3(0.0f, 40.0f, 0.0f));
    camera->SetTopDownView();
    
    auto renderer = std::make_unique<Renderer>();
    auto liquidSim = std::make_unique<LiquidSimulation>(100.0f, 100.0f);
    
    std::cout << "Created simulation with " << liquidSim->GetParticles().size() << " particles" << std::endl;
    
    double lastTime = glfwGetTime();
    const double targetFrameTime = 1.0 / 60.0; // 60 FPS
    const float fixedTimeStep = 1.0f / 60.0f;  // Fixed timestep for physics
    double accumulator = 0.0;
    
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double frameTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Cap frame time to prevent spiral of death
        frameTime = std::min(frameTime, 0.25);
        
        accumulator += frameTime;
        
        // Fixed timestep physics update
        while (accumulator >= targetFrameTime) {
            liquidSim->Update(fixedTimeStep);
            accumulator -= targetFrameTime;
        }
        
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);  // Dark blue background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        renderer->Begin(camera->GetViewMatrix(), camera->GetProjectionMatrix(static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT));
        renderer->RenderLiquid(*liquidSim);
        renderer->RenderWalls(liquidSim->GetWalls());
        renderer->End();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}