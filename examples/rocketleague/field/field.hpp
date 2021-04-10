#ifndef FIELD_HPP_
#define FIELD_HPP_

#include "abcg.hpp"
#include "../camera.hpp"

class Field {
 public:

  void loadModelFromFile(std::string_view path);

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  [[nodiscard]] int getNumTriangles() const {
    return static_cast<int>(m_indices.size()) / 3;
  }

  void paintGL();
  void initializeGL(GLuint program);

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  glm::vec4 color { 58/255.0f, 95/255.0f, 11/255.0f, 1.0f};

  void standardize();
  
};

#endif