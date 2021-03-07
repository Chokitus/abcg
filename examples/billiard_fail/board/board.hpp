#ifndef BOARD_HPP_
#define BOARD_HPP_

#include "abcg.hpp"
#include "../gamedata/gamedata.hpp"
#include "../balls/balls.hpp"

class OpenGLWindow;
class Balls;

class Board {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void checkBounds(Balls &balls);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
};

#endif