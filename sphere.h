#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"

class Sphere : public Object
{
public:
  Sphere(int prec, float angle, const char *tf);
  Sphere(int prec, float angle, const char *tf, const char *nf);

private:
  void createVertices() override;
  int prec = 10;
};

#endif /* SPHERE_H */