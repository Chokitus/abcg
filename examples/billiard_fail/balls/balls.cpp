#include "balls.hpp"
#include <fmt/core.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Balls::initializeGL(GLuint program) {
  terminateGL();

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  // Create balls
  m_balls.clear();
  m_balls.resize(15);

  for (auto &ball : m_balls) {
    ball = createBall();

    // Make sure the ball won't collide with the ship
    do {
      ball.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(ball.m_translation) < 0.5f);
  }
}

void Balls::paintGL() {
  glUseProgram(m_program);

  for (auto &ball : m_balls) {
    glBindVertexArray(ball.m_vao);

    glUniform4fv(m_colorLoc, 1, &ball.m_color.r);
    glUniform1f(m_scaleLoc, ball.m_scale);

    glUniform2f(m_translationLoc, ball.m_translation.x,
                    ball.m_translation.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, ball.m_polygonSides + 2);

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Balls::terminateGL() {
  for (auto ball : m_balls) {
    glDeleteBuffers(1, &ball.m_vbo);
    glDeleteVertexArrays(1, &ball.m_vao);
  }
}

void Balls::update(float deltaTime) {
  for (auto &ball : m_balls) {
    ball.m_translation += ball.m_velocity * deltaTime;

  }
}

Balls::Ball Balls::createBall(glm::vec2 translation,
                                              float scale) {
  Ball ball;

    auto &re{m_randomEngine};  // Shortcut

  // Randomly choose the number of sides
  std::uniform_int_distribution<int> randomSides(6, 20);
  ball.m_polygonSides = 360;

  // Choose a random color (actually, a grayscale)
  std::uniform_real_distribution<float> randomIntensity(0.5f, 1.0f);
  ball.m_color = glm::vec4(1) * randomIntensity(re);

  ball.m_color.a = 1.0f;
  ball.m_scale = scale;
  ball.m_translation = translation;

  // Choose a random direction
  glm::vec2 direction{m_randomDist(re), m_randomDist(re)};
  ball.m_velocity = glm::normalize(direction) / 1.0f;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / ball.m_polygonSides};
  
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  glGenBuffers(1, &ball.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, ball.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &ball.m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(ball.m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, ball.m_vbo);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);

  return ball;
}