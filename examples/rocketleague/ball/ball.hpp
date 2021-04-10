#ifndef BALL_HPP_
#define BALL_HPP_

#include "abcg.hpp"
#include "../camera.hpp"

class Ball {
 public:

  void loadModelFromFile(std::string_view path);

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  [[nodiscard]] int getNumTriangles() const {
    return static_cast<int>(m_indices.size()) / 3;
  }

  void update(float deltaTime);
  void paintGL(Camera m_camera);
  void initializeGL(GLuint program);
  float x();
  float y();
  float z();

  float scale { 0.5f };
  float radius { 0.5f * scale };
  glm::vec3 direction{0.0f, 0.0f, -0.0f};

  glm::mat4 position{1.0f};

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};
  
  
  glm::vec3 gravity{0.0f, -50.0f, 0.0f};

  void createBuffers();
  void standardize();

  void x(float value);
  void y(float value);
  void z(float value);
  
};

#endif