#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

#include "ball/ball.hpp"
#include "field/field.hpp"

// Custom specialization of std::hash injected in namespace std
namespace std {
  template <>
  struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const noexcept {
      std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
      return h1;
    }
  };
}  // namespace std

void OpenGLWindow::handleEvent(SDL_Event& ev) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w)
      m_dollySpeed = 2.0f;
    if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s)
      m_dollySpeed = -2.0f;
    if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a)
      m_panSpeed = -2.0f;
    if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d)
      m_panSpeed = 2.0f;
    if (ev.key.keysym.sym == SDLK_q) m_truckSpeed = -2.0f;
    if (ev.key.keysym.sym == SDLK_e) m_truckSpeed = 2.0f;

    if (ev.key.keysym.sym == SDLK_c) lockCamera = !lockCamera;
  }
  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_q && m_truckSpeed < 0) m_truckSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_e && m_truckSpeed > 0) m_truckSpeed = 0.0f;
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(
    getAssetsPath() + "texture.vert",
    getAssetsPath() + "texture.frag"
  );

  ball.loadModelFromFile(getAssetsPath() + "ball/ball.obj");
  ball.initializeGL(m_program);
  
  duck.loadModelFromFile(getAssetsPath() + "duck/duck.obj");
  duck.initializeGL(m_program);

  ground.loadModelFromFile(getAssetsPath() + "ground/field-ground.obj", -0.079f, 0.975f);
  ground.initializeGL(m_program);

  field.loadModelFromFile(getAssetsPath() + "stadium/stadium.obj", -0.01f);
  field.initializeGL(m_program);
  
  resizeGL(getWindowSettings().width, getWindowSettings().height);
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  ground.paintGL();
  field.paintGL();
  duck.paintGL();
  ball.paintGL(m_camera); 
}

void OpenGLWindow::paintUI() { 
  abcg::OpenGLWindow::paintUI(); 
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_EBO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLWindow::update() {
  float deltaTime { static_cast<float>(getDeltaTime()) };

  duck.update(&ball);
  ball.update(deltaTime);

  if (lockCamera) {
    m_camera.lookAtCar(
      glm::vec3(duck.x(), duck.y(), duck.z()),
      glm::vec3(ball.x(), ball.y(), ball.z())
    );

    duck.move(m_dollySpeed, m_panSpeed, deltaTime);
    return;
  }

  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
}