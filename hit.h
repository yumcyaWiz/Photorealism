#ifndef HIT_H
#define HIT_H
#include "vec3.h"

class Primitive;

class Hit {
  public:
    double t;
    Vec3 hitPos;
    Vec3 hitNormal;
    Primitive* hitPrimitive;

    Hit() {};
};
#endif
