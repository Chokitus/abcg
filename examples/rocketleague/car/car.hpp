#ifndef CAR_HPP_
#define CAR_HPP_

#include "abcg.hpp"
#include "../camera.hpp"
#include "../ball/ball.hpp"

class Car {
 public:

  void loadModelFromFile(std::string_view path);
  //void setupVAO(GLuint program);

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  [[nodiscard]] int getNumTriangles() const {
    return static_cast<int>(m_indices.size()) / 3;
  }

  void update(Ball* ball);
  void paintGL();
  void initializeGL(GLuint program);
  
  float x();
  float y();
  float z();

  void move(float dollySpeed, float panSpeed, float deltaTime);

  float scale { 0.5f };
  float radius { 0.5f * scale };
  float speed { 0.0f };

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};
  glm::mat4 position{1.0f};

  glm::vec3 lookDirection{ 1.0f, 0.0f, 0.0f };
  

  void createBuffers();
  void standardize();

  void x(float value);
  void y(float value);
  void z(float value);
  
};

#endif