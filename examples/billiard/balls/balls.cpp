#include "balls.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <math.h>

glm::vec3 color(int red, int blue, int green) {
  return glm::vec3(red/255.0f, blue/255.0f, green/255.0f);
}

void Balls::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");
  radius = 0.03f;
  
  // Create balls
  m_balls.clear();

  createDefaultBoard();
}

void Balls::paintGL() {
  glUseProgram(m_program);

  for (auto &ball : m_balls) {
    if (ball.beenPocketed) {
      continue;
    }
    glBindVertexArray(ball.m_vao);

    glUniform4fv(m_colorLoc, 1, &ball.m_color.r);
    glUniform1f(m_scaleLoc, radius);

    glUniform2f(m_translationLoc, ball.position.x, ball.position.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 360 + 2);

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

void Balls::update(float deltaTime, GameData* gameData) {
  bool isRunning = false;
  for (auto &ball : m_balls) {
    auto newPosition { ball.position + ball.velocity * deltaTime };

    ball.prevPosition = ball.position;
    ball.position = newPosition;

    auto velocityAmount { glm::length(ball.velocity) };

    auto frictionRatio { pow(0.5f, (velocityAmount - 1))/2.0f };
    ball.velocity -= glm::normalize(ball.velocity) * ((frictionRatio < 0.5f ? 0.5f : frictionRatio) * 0.3f * deltaTime);

    if (velocityAmount < 0.02f) {
      ball.velocity = glm::vec2(0);
    }
    isRunning = isRunning || glm::length(ball.velocity) > 0;
  }
  gameData->m_state = isRunning ? State::Running : State::Playable; 
}

Balls::Ball Balls::createBall(glm::vec2 translation, glm::vec3 color) {
  Ball ball {
    .m_color = glm::vec4(color, 1),
    .position = translation,
    .prevPosition = translation,
    .velocity = glm::vec2(0)
  };

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / 360};
  
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

void Balls::hit(Balls::Ball* ball1, Balls::Ball* ball2) {
	glm::vec2 normal { ball2->position - ball1->position };
  float tmp { radius - glm::length(normal) / 2 };
	normal = glm::normalize(normal);
	
	glm::vec2 normBall1Velocity { normal*glm::dot(normal,ball1->velocity) };
	glm::vec2 tangBall1Velocity { ball1->velocity - normBall1Velocity };

	normal *= -1.0f;
	glm::vec2 normBall2Velocity { normal * glm::dot(normal, ball2->velocity) };
	glm::vec2 tangBall2Velocity { ball2->velocity - normBall2Velocity };
	
	ball1->velocity = (normBall2Velocity + tangBall1Velocity) * 0.98f;	
	ball2->velocity = (normBall1Velocity + tangBall2Velocity) * 0.98f;		
	
	ball1->position = ball1->position + normal*tmp;
	ball2->position = ball2->position - normal*tmp;
}

glm::vec3 Balls::rndColor() {
  return color(m_randomEngine() % 230 + 20, m_randomEngine() % 230 + 20, m_randomEngine() % 230 + 20);
}

void Balls::createDefaultBoard() {
  auto sqt3 { 1.75f };

  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  auto whiteBall { 
    createBall(glm::vec2(-0.5f, 0), color(230, 230, 230))
  };
  whiteBall.isWhite = true;

  white = &whiteBall;

  white->velocity = glm::vec2(0.01f, 0.0f);

  // White
  m_balls.emplace_back(
    whiteBall
  );

  // First Column
  m_balls.emplace_back(
    createBall(glm::vec2(0, 0), glm::vec4(240/255.0f, 181/255.0f, 4/255.0f, 1))
  );

  // Second Column
  m_balls.emplace_back(
    createBall(glm::vec2(sqt3*radius, radius), rndColor())
  );
  m_balls.emplace_back(
    createBall(glm::vec2(sqt3*radius, -radius), rndColor())
  );

  // Third Column
  m_balls.emplace_back(
    createBall(glm::vec2(2*sqt3*radius, 0), rndColor())
  );
  m_balls.emplace_back(
    createBall(glm::vec2(2*sqt3*radius, -2*radius), rndColor())
  );
    m_balls.emplace_back(
    createBall(glm::vec2(2*sqt3*radius, 2*radius), rndColor())
  );

  // Fourth Column
  m_balls.emplace_back(
    createBall(glm::vec2(3*sqt3*radius, radius), rndColor())
  );
  m_balls.emplace_back(
    createBall(glm::vec2(3*sqt3*radius, -radius), rndColor())
  );
  m_balls.emplace_back(
    createBall(glm::vec2(3*sqt3*radius, 3*radius), rndColor())
  );
  m_balls.emplace_back(
    createBall(glm::vec2(3*sqt3*radius, -3*radius), rndColor())
  );

  // Fifth Column
  m_balls.emplace_back(
    createBall(glm::vec2(4*sqt3*radius, 0), rndColor())
  );
  m_balls.emplace_back(
    createBall(glm::vec2(4*sqt3*radius, -2*radius), rndColor())
  );
  m_balls.emplace_back(
    createBall(glm::vec2(4*sqt3*radius, 2*radius), rndColor())
  );
  m_balls.emplace_back(
    createBall(glm::vec2(4*sqt3*radius, 4*radius), rndColor())
  );
  m_balls.emplace_back(
    createBall(glm::vec2(4*sqt3*radius, -4*radius), rndColor())
  );



  
}