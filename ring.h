#ifndef RING_H
#define RING_H
#include "object.h"

class Ring : public Object
{
public:
  Ring(int prec, const char *tf);
  Ring(int prec, const char *tf, const char *nf);
  Ring(int prec, float inner_r, float outer_r, const char *tf);
  Ring(int prec, float inner_r, float outer_r, const char *tf, const char *nf);

private:
  void createVertices() override;

  float inner_r = 2;
  float outer_r = 1;
  int prec = 64;
};

#endif /* RING_H */