#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

class LiquidSimulation;
class Wall;

class Renderer {
public:
  Renderer();
  ~Renderer();

  void Begin(const glm::mat4 &view, const glm::mat4 &projection);
  void RenderLiquid(const LiquidSimulation &simulation);
  void RenderWalls(const std::vector<Wall> &walls);
  void End();

private:
  GLuint CompileShader(const std::string &vertexPath,
                       const std::string &fragmentPath);
  GLuint LoadShaderFromFile(const std::string &path, GLenum shaderType);

  void InitializeLiquidBuffers();
  void InitializeWallBuffers();

  GLuint liquidShader;
  GLuint wallShader;

  GLuint liquidVAO, liquidVBO;
  GLuint wallVAO, wallVBO, wallEBO;

  glm::mat4 currentView;
  glm::mat4 currentProjection;
};