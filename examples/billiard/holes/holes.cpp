#include "holes.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Holes::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");
  radius = 0.08f;
  
  // Create Holes
  m_holes.clear();

  m_holes.emplace_back(
    createHole(glm::vec2(0.70f, 0.50f), M_PI, M_PI * 1.51f)
  );

  m_holes.emplace_back(
    createHole(glm::vec2(0.70f, -0.45f), M_PI * 0.5f, M_PI*1.01f)
  );

  m_holes.emplace_back(
    createHole(glm::vec2(-0.75f, -0.45f), 0, M_PI*0.51f)
  );

  m_holes.emplace_back(
    createHole(glm::vec2(-0.75f, 0.5f), 1.5f*M_PI, M_PI*2.01f)
  );

  m_holes.emplace_back(
    createHole(glm::vec2(0.0f, 0.52f), M_PI, M_PI*2.01f)
  );

    m_holes.emplace_back(
    createHole(glm::vec2(0.0f, -0.48f), 0.0f, M_PI)
  );
}

void Holes::paintGL() {
  glUseProgram(m_program);

  for (auto &hole : m_holes) {
    glBindVertexArray(hole.m_vao);

    glUniform4fv(m_colorLoc, 1, &hole.m_color.r);
    glUniform1f(m_scaleLoc, radius);

    glUniform2f(m_translationLoc, hole.position.x, hole.position.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 180 + 2);

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Holes::terminateGL() {
  for (auto hole : m_holes) {
    glDeleteBuffers(1, &hole.m_vbo);
    glDeleteVertexArrays(1, &hole.m_vao);
  }
}

Holes::Hole Holes::createHole(glm::vec2 translation, double start, double end) {
  Hole hole {
    .m_color = glm::vec4(0.15f, 0.15f, 0.15f, 1),
    .position = translation
  };

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / 180};
  
  for (auto angle : iter::range(start, end, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  glGenBuffers(1, &hole.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, hole.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &hole.m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(hole.m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, hole.m_vbo);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);

  return hole;
}

void Holes::isPocketed(Balls::Ball* ball) {
  for (auto &hole : m_holes) {
    if (glm::distance(ball->position, hole.position) <= 1.1f * radius) {
      ball->velocity = glm::vec2(0);
      if (ball->isWhite) {
        ball->position = glm::vec2(-0.5f, 0);
        return;
      }
      ball->beenPocketed = true;
      return;
    }
  }
}