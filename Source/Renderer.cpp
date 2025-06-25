#include "Renderer.h"
#include "LiquidSimulation.h"
#include "Wall.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {
    std::cout << "Loading shaders..." << std::endl;
    liquidShader = CompileShader("Shaders/liquid.vert", "Shaders/liquid.frag");
    std::cout << "Liquid shader ID: " << liquidShader << std::endl;
    wallShader = CompileShader("Shaders/wall.vert", "Shaders/wall.frag");
    std::cout << "Wall shader ID: " << wallShader << std::endl;
    
    InitializeLiquidBuffers();
    InitializeWallBuffers();
    std::cout << "Renderer initialized" << std::endl;
}

Renderer::~Renderer() {
    glDeleteProgram(liquidShader);
    glDeleteProgram(wallShader);
    
    glDeleteVertexArrays(1, &liquidVAO);
    glDeleteBuffers(1, &liquidVBO);
    
    glDeleteVertexArrays(1, &wallVAO);
    glDeleteBuffers(1, &wallVBO);
    glDeleteBuffers(1, &wallEBO);
}

void Renderer::InitializeLiquidBuffers() {
    glGenVertexArrays(1, &liquidVAO);
    glGenBuffers(1, &liquidVBO);
    
    glBindVertexArray(liquidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, liquidVBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Renderer::InitializeWallBuffers() {
    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glGenBuffers(1, &wallEBO);
    
    glBindVertexArray(wallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Renderer::Begin(const glm::mat4& view, const glm::mat4& projection) {
    currentView = view;
    currentProjection = projection;
}

void Renderer::RenderLiquid(const LiquidSimulation& simulation) {
    const auto& particles = simulation.GetParticles();
    std::cout << "Rendering " << particles.size() << " particles" << std::endl;
    if (particles.empty()) {
        std::cout << "No particles to render!" << std::endl;
        return;
    }
    
    std::vector<float> vertexData;
    vertexData.reserve(particles.size() * 7);
    
    for (const auto& particle : particles) {
        vertexData.push_back(particle.position.x);
        vertexData.push_back(particle.position.y);
        vertexData.push_back(particle.position.z);
        vertexData.push_back(particle.color.r);
        vertexData.push_back(particle.color.g);
        vertexData.push_back(particle.color.b);
        vertexData.push_back(particle.radius * 60.0f);
    }
    
    glUseProgram(liquidShader);
    glUniformMatrix4fv(glGetUniformLocation(liquidShader, "view"), 1, GL_FALSE, glm::value_ptr(currentView));
    glUniformMatrix4fv(glGetUniformLocation(liquidShader, "projection"), 1, GL_FALSE, glm::value_ptr(currentProjection));
    
    glBindVertexArray(liquidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, liquidVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);
    
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(10.0f);  // Fallback size
    std::cout << "Drawing " << particles.size() << " points" << std::endl;
    glDrawArrays(GL_POINTS, 0, particles.size());
    glDisable(GL_PROGRAM_POINT_SIZE);
    
    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err << std::endl;
    }
    
    glBindVertexArray(0);
}

void Renderer::RenderWalls(const std::vector<Wall>& walls) {
    glUseProgram(wallShader);
    glUniformMatrix4fv(glGetUniformLocation(wallShader, "view"), 1, GL_FALSE, glm::value_ptr(currentView));
    glUniformMatrix4fv(glGetUniformLocation(wallShader, "projection"), 1, GL_FALSE, glm::value_ptr(currentProjection));
    
    for (const auto& wall : walls) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        wall.GenerateMesh(vertices, indices);
        
        glm::mat4 model = wall.GetModelMatrix();
        glUniformMatrix4fv(glGetUniformLocation(wallShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(wallVAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
        
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
    }
}

void Renderer::End() {
    glUseProgram(0);
}

GLuint Renderer::CompileShader(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vertexShader = LoadShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = LoadShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

GLuint Renderer::LoadShaderFromFile(const std::string& path, GLenum shaderType) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return 0;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    const char* sourcePtr = source.c_str();
    
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed (" << path << "): " << infoLog << std::endl;
    }
    
    return shader;
}