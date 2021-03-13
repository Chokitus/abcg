#include "openglwindow.hpp"

#include <imgui.h>
#include <unordered_map>
#include <fmt/core.h>

#include "abcg.hpp"

std::unordered_map<std::string, std::string> teste;

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    m_stick.click = !m_stick.drag;
    m_stick.drag = true;
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    m_stick.release = true;
  }
  if (event.type == SDL_MOUSEMOTION) {
    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    m_stick.mousePosition.x = (mousePosition.x / (m_viewportWidth / 2.0f)) - 1.0f;
    m_stick.mousePosition.y = -((mousePosition.y / (m_viewportHeight / 2.0f)) - 1.0f);
  }
}

void OpenGLWindow::initializeGL() {
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  m_stickProgram = createProgramFromFile(getAssetsPath() + "stick.vert",
                                         getAssetsPath() + "stick.frag");

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  glClearColor(0.0f, 0.0f, 0.0f, 1);

#if !defined(__EMSCRIPTEN__)
  glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playable;

  m_board.initializeGL(m_objectsProgram);
  m_balls.initializeGL(m_objectsProgram);
  m_holes.initializeGL(m_objectsProgram);
  m_stick.initializeGL(m_stickProgram);
}

void OpenGLWindow::update() {
  if (m_gameData.m_state == State::Win &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  float deltaTime{static_cast<float>(getDeltaTime())};

  // Se todas pararam, então o modo é Playable
  if (m_gameData.m_state == State::Running) {
    m_balls.update(deltaTime, &m_gameData);
  }

  if (m_gameData.m_state == State::Playable) {
    Balls::Ball* white;
    for (auto &ball : m_balls.m_balls) {
      if (ball.isWhite) {
        white = &ball;
        break;
      }
    }
    if (m_stick.update(white, true, m_balls.radius)) {
      m_gameData.m_state = State::Running;
    }
  }

  if (m_gameData.m_state == State::Running) {
    checkCollisions();
    checkWinCondition();
  }
}

void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  //m_starLayers.paintGL();
  m_board.paintBackground();
  m_holes.paintGL();
  m_board.paintGL();
  m_balls.paintGL();
  m_stick.paintGL();
  
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    auto size{ImVec2(0, 0)};
    auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::Win) {
      ImGui::Text("You Win!");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_stickProgram);
  glDeleteProgram(m_objectsProgram);

  m_balls.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  for (auto &ball1 : m_balls.m_balls) {
    m_board.checkCollision(&ball1, m_balls.radius);
    if (ball1.beenHit || ball1.beenPocketed) continue;
    m_holes.isPocketed(&ball1);

    for (auto &ball2 : m_balls.m_balls) {
      if (ball2.beenHit || ball2.beenPocketed) continue;

      auto ball1Position { ball1.position };
      auto ball2Position { ball2.position };
      auto distance{
        glm::distance(ball1Position, ball2Position)
      };

      if (distance != 0 && distance < m_balls.radius * 2) {
        m_balls.hit(&ball1, &ball2);
      }
    }
  }
  for (auto &ball : m_balls.m_balls) ball.beenHit = false;
}

void OpenGLWindow::checkWinCondition() {
  bool allPocketed = true;
  for (auto &ball : m_balls.m_balls) {
    allPocketed = allPocketed && (ball.isWhite || ball.beenPocketed);
  }
  if (allPocketed) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}