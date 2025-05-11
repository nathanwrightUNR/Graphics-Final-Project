#ifndef RING_H
#define RING_H
#include "object.h"

/**
 * @brief Ring
 *  inherits from object, has fields for inner and outer radius
 *  as well as precision
 *
 *  overrides create vertices like all other objects
 */
class Ring : public Object
{
public:
  Ring(int prec, float inner_r, float outer_r);
  Ring(int prec, float inner_r, float outer_r, const char *tf);
  Ring(int prec, float inner_r, float outer_r, const char *tf, const char *nf);

private:
  void createVertices() override;

  float inner_r = 2;
  float outer_r = 1;
  int prec = 64;
};

#endif /* RING_H */