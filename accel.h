#ifndef ACCEL_H
#define ACCEL_H
#include <memory>
#include <vector>
#include "primitive.h"
class Accel {
  public:
    std::vector<std::shared_ptr<Primitive>> prims;

    Accel(const std::vector<std::shared_ptr<Primitive>>& _prims) : prims(_prims) {};

    virtual bool intersect(const Ray& ray, Hit& res) const = 0;
};
#endif
