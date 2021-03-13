#ifndef STICK_HPP_
#define STICK_HPP_

#include <list>

#include "abcg.hpp"
#include "../gamedata.hpp"
#include "../board/board.hpp"
#include "../balls/balls.hpp"

class OpenGLWindow;

class Stick {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  bool update(Balls::Ball* white, bool isPlayable, float radius);
  void setRotation(float rotation) { m_rotation = rotation; }
  glm::vec2 mousePosition;

  bool click { false };
  bool drag { false };
  bool release { false };

  glm::vec2 mousePositionStart;

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_rotationLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  float m_rotation;

  float width;
  float scale;

  glm::vec2 position;
  glm::vec4 m_color { glm::vec4(94/255.0f, 73/255.0f, 60/255.0f, 1) };
};

#endif
