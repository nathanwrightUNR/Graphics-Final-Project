#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <stack>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "mesh.h"
#include "sphere.h"
#include "ring.h"
#include "skybox.h"

class Graphics
{
public:
  /**
   * @brief construct a new Graphics object
   *
   */
  Graphics();

  /**
   * @brief destroy the Graphics object
   *
   */
  ~Graphics();

  /**
   * @brief
   * initialize Camera with params width and height
   * initialize and add Shaders,and get projection,
   * view and model matrices from shaders.
   *
   * get color and position vectors from the shader
   * and assign to Graphics object.
   *
   * instantiate object assign to m_object, and
   * initialize it with m_vertPos and m_vertCol.
   *
   * enable depth testing.
   *
   * @param width
   * @param height
   * @return true
   * @return false
   */
  bool Initialize(int width, int height);

  Object *getStarship();

  void Update(double dt);

  void generateAsteroidTransforms(std::vector<glm::mat4> &belt,
                                  std::vector<float> &angles,
                                  int count,
                                  float r,
                                  int span);

  void updateAsteroidBelt(std::vector<glm::mat4> &belt, Object *asteroid);

  Object *getClosestPlanet();
  /**
   * @brief
   * clears screen and enables shaders, pass projection
   * and view matrices to the shader and render the object,
   * report errors if any occured.
   */
  void Render();

  Camera *getCamera();

  void SetOrbit(bool);

private:
  // string to record error message.
  std::string ErrorString(GLenum error);

  bool collectShPrLocs();

  stack<glm::mat4> modelStack;
  bool oribing = false;

  // pointer to Camera object.
  Camera *m_camera;
  // pointer to Shader object.
  Shader *m_shader;

  // GLint to store projection matrix.
  GLint m_projectionMatrix;
  // GLint to view projection matrix.
  GLint m_viewMatrix;
  // GLint to store model matrix.
  GLint m_modelMatrix;

  GLint m_positionAttrib;
  GLint m_colorAttrib;
  GLint m_tcAttrib;
  GLint m_hasTexture;
  GLint m_has_nmap;
  GLint m_NpAttrib;

  // changed to vector of object pointers.
  Object *m_sun;
  Object *m_mercury;
  Object *m_venus;
  Object *m_earth;
  Object *m_moon;
  Object *m_mars;
  Object *m_jupiter;
  Object *m_saturn;
  Object *m_saturn_ring;
  Object *m_uranus;
  Object *m_neptune;

  Object *m_starship;

  Object *m_mercury_trace;
  Object *m_venus_trace;
  Object *m_earth_trace;
  Object *m_mars_trace;
  Object *m_jupiter_trace;
  Object *m_saturn_trace;
  Object *m_uranus_trace;
  Object *m_neptune_trace;

  std::vector<glm::mat4> m_inner_asteroid_belt1;
  std::vector<glm::mat4> m_inner_asteroid_belt2;
  std::vector<glm::mat4> m_inner_asteroid_belt3;

  std::vector<float> m_inner_asteroid_angles1;
  std::vector<float> m_inner_asteroid_angles2;
  std::vector<float> m_inner_asteroid_angles3;

  Object *m_inner_asteroid1;
  Object *m_inner_asteroid2;
  Object *m_inner_asteroid3;

  std::vector<glm::mat4> m_outer_asteroid_belt1;
  std::vector<glm::mat4> m_outer_asteroid_belt2;
  std::vector<glm::mat4> m_outer_asteroid_belt3;

  std::vector<float> m_outer_asteroid_angles1;
  std::vector<float> m_outer_asteroid_angles2;
  std::vector<float> m_outer_asteroid_angles3;

  Object *m_outer_asteroid1;
  Object *m_outer_asteroid2;
  Object *m_outer_asteroid3;

  Object *m_halleys;

  // Skybox
  GLuint skyboxVAO;
  GLuint skyboxVBO;
  Skybox *m_skybox;
  GLint m_skyboxProjMatrix;
  GLint m_skyboxViewMatrix;
  GLint m_skyboxPositionAttrib;
  std::vector<GLfloat> skyboxVertices;
  GLuint sbCubeMap;

  GLuint skyboxCubeMap();
};

#endif /* GRAPHICS_H */
