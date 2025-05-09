#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"

class Sphere : public Object
{
public:
  Sphere(int prec, const char *fname);
  Sphere(int prec);

private:
  void createVertices() override;
  int prec = 10;
};

#endif /* SPHERE_H */