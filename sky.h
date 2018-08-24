#ifndef SKY_H
#define SKY_H
#include "vec3.h"
#include "ray.h"
class Sky {
  public:
    virtual RGB getColor(const Ray& ray) const = 0;
};
#endif
