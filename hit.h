#ifndef HIT_H
#define HIT_H
#include "vec3.h"

class Primitive;

class Hit {
  public:
    float t;
    Vec3 hitPos;
    Vec3 hitNormal;
    const Primitive* hitPrimitive;
    float u;
    float v;

    Hit() {
      t = 10000;
      hitPrimitive = nullptr;
    };
};
#endif
