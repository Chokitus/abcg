#include "board.hpp"

void Board::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  initBorder(&left, true);
  initBorder(&up, false);
  initBorder(&down, false);
  initBorder(&right, true);

  // Create geometry
  std::vector<glm::vec2> positions(0);

  positions.emplace_back(0.0f, 0.0f);
  positions.emplace_back(0.0f, left.length);
  positions.emplace_back(up.length, left.length);
  positions.emplace_back(up.length, 0.0f);
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

void Board::paintGL() {
  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glUniform1f(m_scaleLoc, 0.5f);

  glUniform2f(m_translationLoc, left.position.x, left.position.y);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

  glBindVertexArray(0);

  for (auto &border : {left, right, up, down}) {
    glBindVertexArray(border.m_vao);

    glUniform4fv(m_colorLoc, 1, &border.m_color.r);
    glUniform1f(m_scaleLoc, 0.5f);

    glUniform2f(m_translationLoc, border.position.x, border.position.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Board::terminateGL() {
  glDeleteBuffers(1, &left.m_vbo);
  glDeleteVertexArrays(1, &left.m_vao);

  glDeleteBuffers(1, &right.m_vbo);
  glDeleteVertexArrays(1, &right.m_vao);

  glDeleteBuffers(1, &up.m_vbo);
  glDeleteVertexArrays(1, &up.m_vao);

  glDeleteBuffers(1, &down.m_vbo);
  glDeleteVertexArrays(1, &down.m_vao);
}

void Board::initBorder(Border* border, bool isSide) {

  // Create geometry
  std::vector<glm::vec2> positions(0);

  positions.emplace_back(0.0f, 0.0f);
  if (isSide) {
    positions.emplace_back(0.0f, border->length);
    positions.emplace_back(0.1f, border->length);
    positions.emplace_back(0.1f, 0.0f);
  } else {
    positions.emplace_back(border->length, 0.0f);
    positions.emplace_back(border->length, 0.1f);
    positions.emplace_back(0.0f, 0.1f);
  }

  positions.push_back(positions.at(1));

   // Generate VBO
  glGenBuffers(1, &border->m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, border->m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &border->m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(border->m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, border->m_vbo);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Board::checkCollision(Balls::Ball* ball, float radius) {
  if (ball->position.x - radius < left.position.x + 0.1f * 0.5f){
    ball->position.x += 0.001f;
    ball->velocity.x = -ball->velocity.x * 0.99f;
  } 
  if (ball->position.x + radius > right.position.x) {
    ball->position.x -= 0.001f;
    ball->velocity.x = -ball->velocity.x * 0.99f;
  }
  if (ball->position.y - radius < down.position.y + 0.1f * 0.5f) {
    ball->position.y += 0.001f;
    ball->velocity.y = -ball->velocity.y * 0.99f;
  }
  if (ball->position.y + radius > up.position.y) {
    ball->position.y -= 0.001f;
    ball->velocity.y = -ball->velocity.y * 0.99f;
  }
}