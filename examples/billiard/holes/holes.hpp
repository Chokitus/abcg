#ifndef HOLES_HPP_
#define HOLES_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "../gamedata.hpp"
#include "../balls/balls.hpp"

class OpenGLWindow;

class Holes {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();
  void isPocketed(Balls::Ball* ball);
  struct Hole {
    GLuint m_vao{};
    GLuint m_vbo{};

    glm::vec4 m_color { 1 };
    glm::vec2 position{glm::vec2(0)};
  };
  
 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};
  float radius{};

  std::list<Hole> m_holes;
  Hole createHole(glm::vec2 translation, double start, double end);
};

#endif
