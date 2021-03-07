#ifndef BALLS_HPP_
#define BALLS_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "../gamedata/gamedata.hpp"
#include "../board/board.hpp"

class OpenGLWindow;

class Balls {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  struct Ball {
    GLuint m_vao{};
    GLuint m_vbo{};

    glm::vec4 m_color{1};
    bool m_hit{false};
    int m_polygonSides{};
    float m_scale{};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};
  };

  std::list<Ball> m_balls;

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};

  Balls::Ball createBall(glm::vec2 translation = glm::vec2(0),
                                     float scale = 0.15f);
};

#endif