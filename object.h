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

  /**
   * @brief
   * binds VAO and VBOs, enable vertex atribute arrays
   * with color and position, calls Draw and then disables
   * color and position for next Render call.
   *
   * @param posAttribLoc
   * @param colAttribLoc
   */
  virtual void Render(GLint posAttribLoc, GLint colAttribLoc);

  virtual void Render(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTex);

  /**
   * @brief get the model.
   *
   * @return glm::mat4
   */
  glm::mat4 GetModel();

  virtual GLuint getTextureID() { return 0; }

  bool hasTex;

protected:
  /**
   * @brief declares vertices and indices of the Object.
   *
   */
  virtual void createVertices() = 0;

  Texture *m_texture = nullptr;

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

  // angle for rotation.
  float angle;
};

#endif /* OBJECT_H */
