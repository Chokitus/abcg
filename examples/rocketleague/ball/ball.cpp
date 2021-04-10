#include "ball.hpp"

#include <fmt/core.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <filesystem>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

namespace std {
  template <>
  struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const noexcept {
      std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
      return h1;
    }
  };
}

void Ball::initializeGL(GLuint program) {
  position = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  position = glm::translate(position, glm::vec3(-10.0f, 0.0f, -5.0f));

  m_program = program;

  // Generate VBO
  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
               m_vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(),
               m_indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Ball::update(float deltaTime) {
  position = glm::translate(position, direction * deltaTime);

  if (y() > 10.0f) {
    y(10.0f);
    direction.y = -direction.y;
  }
 
  // Check borders
  if (x() < -10.5f) {
    x(-10.5f);
    direction.x = -direction.x;
  }

  if (x() > 10.0f) {
    x(10.0f);
    direction.x = -direction.x;
  }

  if (z() < -5.45f) {
    z(-5.45f);
    direction.z = -direction.z;
  }

  if (z() > 5.45f) {
    z(5.45f);
    direction.z = -direction.z;
  }

  auto friction { -1/(1 + pow(2.7182f, -1.5f*deltaTime)) + 1.5 };

  direction.x *= friction;
  direction.z *= friction;

  if (abs(direction.x) <= 0.5f) {
    direction.x = 0.0f;
  }

  if (abs(direction.z) <= 0.5f) {
    direction.z = 0.0f;
  }

  // If it is under the ground
  if (y() < 0.0f) {
    
    // Teleports to the ground level
    y(0.0f);

    // Reverses the speed (impact with the ground)
    direction.y = abs(direction.y) * 0.8f;
    return;
  }
  
  // If it is stopping
  if (y() <= 0.005f && direction.y <= 0.0f && direction.y >= -0.001f) {

    // Teleports to the ground level
    y(0.0f);
    
    // Stops it
    direction.y = 0.0f;
    return;
  }

  direction += gravity * deltaTime;
}

void Ball::paintGL(Camera m_camera) {
  glUseProgram(m_program);
  glBindVertexArray(m_VAO);

  m_camera.init(m_program);

  // Get location of uniform variables (could be precomputed)
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint colorLoc{glGetUniformLocation(m_program, "color")};

  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &position[0][0]);
  glUniform4f(colorLoc, 1.0f, 192/255.0f, 203/255.0f, 1.0f);
  glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
  glUseProgram(0);
}

void Ball::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = "";  // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over faces(polygon)
    size_t indexOffset{0};
    for (const auto faceNumber :
         iter::range(shape.mesh.num_face_vertices.size())) {
      // Number of vertices composing face f
      std::size_t numFaceVertices{shape.mesh.num_face_vertices[faceNumber]};
      // Loop over vertices in the face
      std::size_t startIndex{};
      for (const auto vertexNumber : iter::range(numFaceVertices)) {
        // Access to vertex
        tinyobj::index_t index{shape.mesh.indices[indexOffset + vertexNumber]};

        // Vertex coordinates
        startIndex = 3 * index.vertex_index;
        tinyobj::real_t vx = attrib.vertices[startIndex + 0];
        tinyobj::real_t vy = attrib.vertices[startIndex + 1];
        tinyobj::real_t vz = attrib.vertices[startIndex + 2];

        Vertex vertex{};
        vertex.position = {vx, vy, vz};

        // If uniqueVertices doesn't contain this vertex
        if (hash.count(vertex) == 0) {
          // Add this index (size of m_vertices)
          hash[vertex] = m_vertices.size();
          // Add this vertex
          m_vertices.push_back(vertex);
        }

        m_indices.push_back(hash[vertex]);
      }
      indexOffset += numFaceVertices;
    }
  }
  standardize();
}

float Ball::x() {
  return position[3][0];
}

float Ball::y() {
  return position[3][1];
}

float Ball::z() {
  return position[3][2];
}

void Ball::x(float value) {
  position[3][0] = value;
}

void Ball::y(float value) {
  position[3][1] = value;
}

void Ball::z(float value) {
  position[3][2] = value;
}

void Ball::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto& vertex : m_vertices) {
    max.x = std::max(max.x, vertex.position.x);
    max.y = std::max(max.y, vertex.position.y);
    max.z = std::max(max.z, vertex.position.z);
    min.x = std::min(min.x, vertex.position.x);
    min.y = std::min(min.y, vertex.position.y);
    min.z = std::min(min.z, vertex.position.z);
  }

  // Center and scale
  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;

    // Makes y centered relative to the field
    vertex.position.y += 0.1f;
  }
}