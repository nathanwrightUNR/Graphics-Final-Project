#ifndef MESH_H
#define MESH_H

#include "object.h"

class Mesh : public Object
{
public:
  Mesh();
  Mesh(glm::vec3 pivot, const char *fname);
  Mesh(glm::vec3 pivot, const char *fname, const char *tname);

private:
  void createVertices() override;
  bool loadModelFromFile(const char *path);
  GLuint getTextureID() override { return m_texture ? m_texture->getTextureID() : 0; }
};

#endif /* MESH_H */