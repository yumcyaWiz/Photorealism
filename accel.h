#ifndef ACCEL_H
#define ACCEL_H
#include <memory>
#include <vector>
#include "primitive.h"
class Accel {
  public:
    std::vector<std::shared_ptr<Primitive>> primitives;

    Accel() {};

    void add(const std::shared_ptr<Primitive>& prim) {
      primitives.push_back(prim);
    };
    virtual bool intersect(const Ray& ray, Hit& res) const = 0;
};
#endif
