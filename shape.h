#ifndef SHAPE_H
#define SHAPE_H
#include "ray.h"
#include "hit.h"
class Shape {
  public:
    virtual bool intersect(const Ray& ray, Hit& res) const = 0;
};
#endif
