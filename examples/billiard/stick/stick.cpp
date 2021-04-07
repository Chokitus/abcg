#include "stick.hpp"

void Stick::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
 
  scale = 0.3f;
  width = 0.1f;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0.0f, 0.0f);
  positions.emplace_back(0.0f, 2.5f); 
  positions.emplace_back(width, 2.5f);
  positions.emplace_back(width, 0.0f);
  positions.push_back(positions.at(1));

  // Generate VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Stick::paintGL() {
  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glUniform1f(m_scaleLoc, scale);
  glUniform1f(m_rotationLoc, m_rotation);

  glUniform2f(m_translationLoc, position.x, position.y);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 5);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Stick::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

bool Stick::update(Balls::Ball* white, bool isPlayable, float radius) {
  if (isPlayable) {
    position = white->position;
    glm::vec2 direction {
      glm::vec2 {
        mousePosition.x - white->position.x,
        white->position.y - mousePosition.y
      }
    };
    direction.y = -direction.y;
    auto normalized { glm::normalize(direction) };

    glm::vec2 perp { 
      glm::normalize(
        normalized.x > 0 
        ? glm::vec2(-normalized.y / normalized.x,                                      1) 
        : glm::vec2( normalized.y /(normalized.x < -0.001f ? normalized.x : -0.001f), -1))
    };

    position += normalized * radius + (perp * (width / 2.0f) * scale); // Move back
    m_rotation = std::atan2(direction.y, direction.x) - M_PI_2;

    if (release) {
      release = false;
      drag = false;

      auto mousePositionEnd { mousePosition };

      float force { glm::distance(mousePositionStart, mousePositionEnd) * 5 };
      white->velocity = - normalized * force;
      return true;
    } else if(drag) {
      if (click) {
        mousePositionStart = mousePosition;
        click = false;
      }
      
      auto mousePositionEnd { mousePosition };
      position += normalized * glm::distance(mousePositionStart, mousePositionEnd);
    }
  }
  return false;
}