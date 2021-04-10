#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "abcg.hpp"

class OpenGLWindow;

struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};

  bool operator==(const Vertex& other) const noexcept {
    static const auto epsilon{std::numeric_limits<float>::epsilon()};
    return glm::all(glm::epsilonEqual(position, other.position, epsilon)) &&
           glm::all(glm::epsilonEqual(normal, other.normal, epsilon));
  }
};

class Camera {
 public:

  void init(GLuint m_program);
  void computeViewMatrix();
  void computeProjectionMatrix(int width, int height);

  void dolly(float speed);
  void truck(float speed);
  void pan(float speed);

  void lookAtCar(glm::vec3 carPosition, glm::vec3 ballPosition);

 private:
  friend OpenGLWindow;

  glm::vec3 m_eye{ glm::vec3(0.0f, 0.5f, 2.5f) };  // Camera position
  glm::vec3 m_at { glm::vec3(0.0f, 0.5f, 0.0f) };  // Look-at point
  glm::vec3 m_up { glm::vec3(0.0f, 1.0f, 0.0f) };  // "up" direction

  // Matrix to change from world space to camera soace
  glm::mat4 m_viewMatrix;

  // Matrix to change from camera space to clip space
  glm::mat4 m_projMatrix;
};

#endif