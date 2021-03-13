#ifndef BOARD_HPP_
#define BOARD_HPP_

#include "abcg.hpp"
#include "../balls/balls.hpp"

class OpenGLWindow;
class Balls;

class Board {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void paintBackground();
  void terminateGL();
  
 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  glm::vec4 m_color { 147/255.0f, 184/255.0f, 115/255.0f, 1 };

  struct Border {
    GLuint m_vao{};
    GLuint m_vbo{};

    glm::vec4 m_color { 130/255.0f, 109/255.0f, 64/255.0f, 1 };
    float length;
    glm::vec2 position;
  };

  Border left {
    .length = 2.0f,
    .position = glm::vec2(-0.8f, -0.5f)
  };
  Border right {
    .length = 2.0f,
    .position = glm::vec2( 0.7f, -0.5f)
  };
  Border up {
    .length = 3.1f,
    .position = glm::vec2(-0.8f, 0.5f)
  };
  Border down {
    .length = 3.0f,
    .position = glm::vec2(-0.8f, -0.5f)
  };

  void initBorder(Border* border, bool isSide);
  void checkCollision(Balls::Ball* ball, float radius);
};

#endif