#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"

class Sphere : public Object
{
public:
  Sphere(glm::vec3 color_1, glm::vec3 color_2);
  Sphere(int prec, const char *fname);
  Sphere(int prec);

  void Render(GLint positionAttribLoc, GLint colorAttribLoc, GLint tcAttribLoc, GLint hasTex) override;

private:
  void createVertices() override;
  GLuint getTextureID() override { return m_texture ? m_texture->getTextureID() : 0; }
  // i use two colors to break up solid colors for each sphere
  // red-yellow for sun, blue-green for planet
  int prec = 10;
  glm::vec3 color_1;
  glm::vec3 color_2;
};

#endif /* SPHERE_H */