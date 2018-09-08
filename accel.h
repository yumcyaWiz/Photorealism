#ifndef ACCEL_H
#define ACCEL_H
#include <memory>
#include <vector>
#include "primitive.h"
#include "aabb.h"
template <typename T>
class Accel {
  public:
    std::vector<std::shared_ptr<T>> prims;
    AABB aabb;

    Accel(const std::vector<std::shared_ptr<T>>& _prims) : prims(_prims) {
      for(const auto& prim : prims) {
        aabb = mergeAABB(aabb, prim->worldBound());
      }
    };

    virtual bool intersect(const Ray& ray, Hit& res) const = 0;

    AABB worldBound() const {
      return aabb;
    };
};
#endif
