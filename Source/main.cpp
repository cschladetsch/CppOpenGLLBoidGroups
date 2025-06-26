#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <random>
#include <omp.h>
#include <glm/glm.hpp>
#include "LiquidSimulation.h"
#include "Camera.h"
#include "Renderer.h"
#include "Config.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int error, const char* description);

int main() {
    // Load simple JSON config
    Config config = Config::Load();
    
    std::cout << "?? Starting C++ Liquid Simulation with " << config.particleCount << " particles\n";
    
    // WSL-specific display setup
    bool isWSL = false;
    if (getenv("WSL_DISTRO_NAME") || getenv("WSLENV")) {
        isWSL = true;
        std::cout << "?? WSL detected - applying WSL-specific window settings\n";
        
        // Set WSL-friendly display if not set
        if (!getenv("DISPLAY")) {
            setenv("DISPLAY", ":0", 1);
            std::cout << "Set DISPLAY=:0 for WSL\n";
        }
    }
    
    // Initialize GLFW with better error handling
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // WSL-specific window hints
    if (isWSL) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
        // Don't try fullscreen in WSL - it often fails
    } else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    }

    // WSL-optimized window creation
    GLFWwindow* window = nullptr;
    
    if (isWSL) {
        // WSL: Create large windowed mode (fullscreen often problematic)
        std::cout << "Creating WSL-optimized window...\n";
        window = glfwCreateWindow(1920, 1080, "C++ Liquid Simulation (WSL)", nullptr, nullptr);
        
        if (!window) {
            std::cout << "Large WSL window failed, trying standard size...\n";
            window = glfwCreateWindow(1600, 900, "C++ Liquid Simulation (WSL)", nullptr, nullptr);
        }
        
        if (!window) {
            std::cout << "Standard WSL window failed, trying safe size...\n";
            window = glfwCreateWindow(1280, 720, "C++ Liquid Simulation (WSL)", nullptr, nullptr);
        }
    } else {
        // Native Linux: Try fullscreen first
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = nullptr;
        if (monitor) {
            mode = glfwGetVideoMode(monitor);
            std::cout << "Monitor resolution: " << mode->width << "x" << mode->height << "\n";
            window = glfwCreateWindow(mode->width, mode->height, "C++ Liquid Simulation - FULLSCREEN", monitor, nullptr);
        }
        
        if (!window && mode) {
            std::cout << "Fullscreen failed, trying maximized window...\n";
            glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
            window = glfwCreateWindow(mode->width - 100, mode->height - 100, "C++ Liquid Simulation - MAXIMIZED", nullptr, nullptr);
        }
        
        if (!window) {
            std::cout << "Maximized failed, trying large window...\n";
            window = glfwCreateWindow(1920, 1080, "C++ Liquid Simulation", nullptr, nullptr);
        }
    }
    
    if (!window) {
        std::cerr << "Failed to create any GLFW window - checking display configuration\n";
        std::cerr << "DISPLAY=" << (getenv("DISPLAY") ? getenv("DISPLAY") : "not set") << "\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1); // Enable vsync for stability

    // Get initial window size and set viewport
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    
    // WSL-specific viewport handling
    if (isWSL) {
        // In WSL, sometimes the framebuffer size differs from window size
        int actualWidth, actualHeight;
        glfwGetWindowSize(window, &actualWidth, &actualHeight);
        std::cout << "WSL Window size: " << actualWidth << "x" << actualHeight << std::endl;
        std::cout << "WSL Framebuffer size: " << windowWidth << "x" << windowHeight << std::endl;
        
        // Use the larger of the two for viewport
        windowWidth = std::max(windowWidth, actualWidth);
        windowHeight = std::max(windowHeight, actualHeight);
    }
    
    glViewport(0, 0, windowWidth, windowHeight);
    std::cout << "Initial viewport set to: " << windowWidth << "x" << windowHeight << std::endl;

    // Optional: Hide cursor for immersive experience (only in fullscreen)
    GLFWmonitor* currentMonitor = glfwGetWindowMonitor(window);
    if (currentMonitor) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        std::cout << "Fullscreen mode - cursor hidden\n";
    }

    // Initialize GLEW with error checking
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewError) << std::endl;
        glfwTerminate();
        return -1;
    }

    // Setup OpenGL with explicit viewport settings and brighter colors
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Force viewport to use entire window - critical for full space usage
    glViewport(0, 0, windowWidth, windowHeight);
    
    // Much brighter background color
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);  // Very dark blue instead of black
    
    GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
        std::cerr << "OpenGL error during setup: " << glError << std::endl;
    }
    
    // Print GL viewport state
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    std::cout << "GL Viewport set to: " << viewport[0] << "," << viewport[1] << " " << viewport[2] << "x" << viewport[3] << std::endl;
    
    // Verify we're using full window space
    if (viewport[2] != windowWidth || viewport[3] != windowHeight) {
        std::cout << "??  Viewport mismatch detected! Forcing correct size..." << std::endl;
        glViewport(0, 0, windowWidth, windowHeight);
    }

    // Create simulation using config with memory monitoring
    std::cout << "Creating simulation with " << config.particleCount << " particles...\n";
    LiquidSimulation simulation(config.width, config.height);
    simulation.SetGravity(glm::vec3(0.0f, config.gravity, 0.0f));
    simulation.SetDamping(config.damping);
    
    Camera camera(config.cameraPos);
    camera.SetTarget(config.cameraTarget);
    
    Renderer renderer;

    // Generate particles across the massive area to fill entire window
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posX(5.0f, config.width - 5.0f);   // Use massive width
    std::uniform_real_distribution<float> posY(5.0f, config.height - 5.0f);  // Use massive height  
    std::uniform_real_distribution<float> posZ(-15.0f, 15.0f);               // Deeper for perspective
    std::uniform_real_distribution<float> vel(-3.0f, 3.0f);                  // Higher velocities
    std::uniform_real_distribution<float> sizeRange(1.0f, 3.0f);             // Larger particles
    
    // Much brighter color palette for visibility
    std::vector<glm::vec3> colors = {
        glm::vec3(1.0f, 0.4f, 0.4f),  // Bright Red
        glm::vec3(0.4f, 1.0f, 0.4f),  // Bright Green  
        glm::vec3(0.4f, 0.4f, 1.0f),  // Bright Blue
        glm::vec3(1.0f, 1.0f, 0.4f),  // Bright Yellow
        glm::vec3(1.0f, 0.4f, 1.0f),  // Bright Magenta
        glm::vec3(0.4f, 1.0f, 1.0f),  // Bright Cyan
        glm::vec3(1.0f, 0.7f, 0.2f),  // Orange
        glm::vec3(0.8f, 0.2f, 1.0f)   // Purple
    };
    
    std::cout << "? Generating " << config.particleCount << " particles with SMP acceleration...\n";
    
    // Create particles in smaller batches to avoid memory spikes
    const int batchSize = 2500;
    for (int batch = 0; batch < config.particleCount; batch += batchSize) {
        int endBatch = std::min(batch + batchSize, config.particleCount);
        
        for (int i = batch; i < endBatch; ++i) {
            glm::vec3 position(posX(gen), posY(gen), posZ(gen));
            glm::vec3 velocity(vel(gen), vel(gen) * 0.8f, vel(gen) * 0.3f);
            
            // Make colors even brighter for visibility
            glm::vec3 baseColor = colors[i % colors.size()];
            glm::vec3 colorVariation(
                std::uniform_real_distribution<float>(-0.1f, 0.3f)(gen),  // Bias toward brighter
                std::uniform_real_distribution<float>(-0.1f, 0.3f)(gen),
                std::uniform_real_distribution<float>(-0.1f, 0.3f)(gen)
            );
            glm::vec3 finalColor = glm::clamp(baseColor + colorVariation, 0.2f, 1.0f);  // Minimum brightness
            
            simulation.AddParticle(position, velocity, finalColor);
        }
        
        // Progress feedback for large particle counts
        if (endBatch % 10000 == 0) {
            float progress = (float)endBatch / config.particleCount * 100.0f;
            std::cout << "Generated " << endBatch << " particles (" << std::fixed << std::setprecision(1) << progress << "%)\n";
        }
    }
    
    std::cout << "? Simulation started with " << simulation.GetParticleCount() << " particles\n";
    std::cout << "?? Controls: ESC to exit, Mouse to look around\n";

    // Performance tracking with stability monitoring
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float totalTime = 0.0f;
    int frameCounter = 0;
    
    // Main render loop with stability checks
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        totalTime += deltaTime;
        frameCounter++;
        
        // Clamp deltaTime for stability
        deltaTime = std::clamp(deltaTime, 0.001f, 0.033f);  // 30-1000 FPS range
        
        // Exit on ESC key
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            std::cout << "ESC pressed, exiting...\n";
            glfwSetWindowShouldClose(window, true);
        }
        
        // Update simulation with error handling
        try {
            simulation.Update(deltaTime);
        } catch (const std::exception& e) {
            std::cerr << "Simulation error: " << e.what() << std::endl;
            break;
        }
        
        // Clear and render with proper viewport management
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // CRITICAL: Always get current framebuffer size and force viewport to full size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        // Force viewport to use ENTIRE window - this is key for full space usage
        glViewport(0, 0, width, height);
        
        // Verify viewport is actually set correctly
        GLint currentViewport[4];
        glGetIntegerv(GL_VIEWPORT, currentViewport);
        if (currentViewport[2] != width || currentViewport[3] != height) {
            std::cout << "?? Correcting viewport: " << currentViewport[2] << "x" << currentViewport[3] 
                      << " -> " << width << "x" << height << std::endl;
            glViewport(0, 0, width, height);
        }
        
        // CRITICAL: Force projection matrix to use current aspect ratio for full window
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        
        // Create projection matrix that fills entire window
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 200.0f);
        
        // Debug: Print window size occasionally with projection info
        static int sizeCheckCounter = 0;
        if (++sizeCheckCounter % 300 == 0) {
            std::cout << "Window: " << width << "x" << height << " | Viewport: " 
                      << currentViewport[2] << "x" << currentViewport[3] 
                      << " | Aspect: " << aspectRatio 
                      << " | FOV: 60� | Particles: " << simulation.GetParticleCount() << std::endl;
        }
        
        // Render with explicit projection matrix for full window coverage
        try {
            renderer.Begin(camera.GetViewMatrix(), projection);
            renderer.RenderLiquid(simulation);
            renderer.End();
        } catch (const std::exception& e) {
            std::cerr << "Rendering error: " << e.what() << std::endl;
            break;
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Performance stats every 10 seconds
        static int statsFrameCount = 0;
        if (++statsFrameCount % 600 == 0) {
            float avgFPS = frameCounter / totalTime;
            int numThreads = omp_get_max_threads();
            std::cout << "?? PERFORMANCE: " << static_cast<int>(avgFPS) << " FPS avg | " 
                      << simulation.GetParticleCount() << " particles | "
                      << numThreads << " CPU cores | "
                      << width << "x" << height << "\n";
        }
    }

    // Save config on exit
    config.Save();
    
    std::cout << "?? Simulation ended successfully. Runtime: " << std::fixed << std::setprecision(1) << totalTime << " seconds\n";
    std::cout << "? SMP performance with " << omp_get_max_threads() << " CPU cores utilized\n";
    
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Update viewport to use full window size
    glViewport(0, 0, width, height);
    std::cout << "Viewport resized to: " << width << "x" << height << std::endl;
}

void error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}
