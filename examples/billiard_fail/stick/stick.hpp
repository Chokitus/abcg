#ifndef STICK_HPP_
#define STICK_HPP_

#include <list>

#include "abcg.hpp"
#include "../gamedata/gamedata.hpp"
#include "../board/board.hpp"

class OpenGLWindow;

class Stick {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(Board &board, const GameData &gameData, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  
  // TODO: Define color of this ball
  GLint m_colorLoc{};

  // TODO: Define position of ball?
  GLint m_translationLoc{};

  // TODO: Define position of ball?
  GLint m_rotationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
};

#endif