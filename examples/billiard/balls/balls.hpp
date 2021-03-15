#ifndef BALLS_HPP_
#define BALLS_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "../gamedata.hpp"

class OpenGLWindow;

class Balls {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();
  void update(float deltaTime, GameData* gameData);
  struct Ball {
    GLuint m_vao{};
    GLuint m_vbo{};

    glm::vec4 m_color { 1 };

    bool isWhite { false };

    // Means that we already calculated this interaction
    bool beenHit { false };
    bool beenPocketed { false };
    glm::vec2 position{glm::vec2(0)};
    glm::vec2 prevPosition{glm::vec2(0)};
    glm::vec2 velocity{glm::vec2(0)};
  };
  Ball* white { nullptr };
  
 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};
  float radius{};

  std::list<Ball> m_balls;
  
  // Should keep from its energy
  float frictionEffect = 0.3f;
  // For each of this unit travelled
  float frictionUnit = 0.025;

  std::default_random_engine m_randomEngine;
  
  Balls::Ball createBall(glm::vec2 translation, glm::vec3 color);
  glm::vec3 rndColor();
  void hit(Balls::Ball* ball1, Balls::Ball* ball2);
  void createDefaultBoard();
};

#endif
