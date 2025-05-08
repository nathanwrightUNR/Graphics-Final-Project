#ifndef RING_H
#define RING_H
#include "object.h"

class Ring : public Object
{
public:
  Ring(int prec, const char *path);
  Ring(int prec, float inner_r, float outer_r, const char *path);
  void Render(GLint positionAttribLoc, GLint colorAttribLoc, GLint tcAttribLoc, GLint hasTex) override;

private:
  void createVertices() override;
  GLuint getTextureID() override { return m_texture ? m_texture->getTextureID() : 0; }

  float inner_r = 2;
  float outer_r = 1;
  int prec = 0;
};

#endif /* RING_H */