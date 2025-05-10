#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "graphics_headers.h"
#include "texture.h"

class Object
{
public:
  /**
   * @brief construct a new Object object.
   *
   */
  Object() = default;

  /**
   * @brief destroy the Object object.
   *
   */
  ~Object();

  /**
   * @brief
   * InitBuffers Object with position and color.
   *
   * @param posAttribLoc
   * @param colAttribLoc
   */
  bool InitBuffers();

  void Update(glm::mat4 model_matrix);

  void setupModelMatrix(glm::vec3 pivotLoc, float angle, float scale);

  void Render(GLint posAttribLoc, GLint colAttribLoc,
              GLint tcAttribLoc, GLint hasTex,
              GLint nmAttribLoc, GLint hasNmap);

  void Instance(std::vector<glm::mat4> &transforms);

  void UpdateInstanceBuffer(std::vector<glm::mat4> &transforms);

  void Move(glm::vec3 direction, float speed, float dt);

  glm::vec3 getPosition();

  /**
   * @brief get the model.
   *
   * @return glm::mat4
   */
  glm::mat4 GetModel();

  bool hasTex = false;
  GLuint getTextureID() { return m_texture ? m_texture->getTextureID() : 0; }

  bool hasNmap = false;
  GLuint getNormalMapID() { return m_normal_map ? m_normal_map->getTextureID() : 0; }

  // angle for rotation.
  float angle;

protected:
  /**
   * @brief declares vertices and indices of the Object.
   *
   */
  virtual void createVertices() = 0;

  Texture *m_texture = nullptr;

  Texture *m_normal_map = nullptr;

  glm::vec3 pivotLocation;

  // 4x4 model matrix
  glm::mat4 model;
  /*
   * vector of Vertex instances, containing
   * vec3 color and positional attributes
   */
  std::vector<Vertex> Vertices;

  // store indices as unsigned 32-bit integer
  std::vector<unsigned int> Indices;

  // Vertex Array Object, stores the vertex attribute state
  GLuint vao;

  // Vertex Buffer Object, stores positions, colors of vertices
  GLuint VB;

  // Index Buffer Object, stores the indices for drawing
  GLuint IB;

  glm::vec3 m_position = glm::vec3(0.f);

  GLuint iVB = 0;
  int num_instances = 0;
  bool instanced = false;
};

#endif /* OBJECT_H */
