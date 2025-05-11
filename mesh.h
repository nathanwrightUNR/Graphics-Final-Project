#ifndef MESH_H
#define MESH_H

#include "object.h"

class Mesh : public Object
{
public:
  Mesh();
  // when only given obj
  Mesh(glm::vec3 pivot, const char *fname);
  // when given texture and obj file
  Mesh(glm::vec3 pivot, const char *fname, const char *tname);

private:
  // override create vertices, calls loadModelFromFile
  void createVertices() override;
  bool loadModelFromFile(const char *path);
};

#endif /* MESH_H */