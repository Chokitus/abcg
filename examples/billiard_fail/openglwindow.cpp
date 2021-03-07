#include "openglwindow.hpp"

#include <imgui.h>

#include "abcg.hpp"

void OpenGLWindow::initializeGL() {
  // Load a new font
  auto &io { 
    ImGui::GetIO() 
  };

  auto filename { 
    getAssetsPath() + "Inconsolata-Medium.ttf" 
  };
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the stars
  m_ballProgram = createProgramFromFile(
    getAssetsPath() + "ball.vert",
    getAssetsPath() + "ball.frag"
  );
  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(
    getAssetsPath() + "objects.vert", 
    getAssetsPath() + "objects.frag"
  );

  glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playable;

  m_balls.initializeGL(m_ballProgram);
  //m_ship.initializeGL(m_objectsProgram);
  //m_asteroids.initializeGL(m_objectsProgram, 3);
  //m_bullets.initializeGL(m_objectsProgram);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playable &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  //m_ship.update(m_gameData, deltaTime);
  //m_starLayers.update(m_ship, deltaTime);
  m_balls.update(deltaTime);
  //m_bullets.update(m_ship, m_gameData, deltaTime);

  if (m_gameData.m_state == State::Playable) {
    checkCollisions();
    checkWinCondition();
  }
}

void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_balls.paintGL();
  //m_asteroids.paintGL();
  //m_bullets.paintGL();
  //m_ship.paintGL(m_gameData);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    auto size{ImVec2(300, 85)};
    auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    ImGui::PopFont();
    ImGui::End();
  }
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_ballProgram);
  glDeleteProgram(m_objectsProgram);

  m_balls.terminateGL();
  //m_bullets.terminateGL();
  //m_ship.terminateGL();
  //m_starLayers.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  
}

void OpenGLWindow::checkWinCondition() {

}