#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <memory>
#include "shape.h"
#include "material.h"
#include "light.h"
class Primitive {
  public:
    std::shared_ptr<Shape> shape;
    std::shared_ptr<Material> material;

    Primitive(const std::shared_ptr<Shape>& _shape, const std::shared_ptr<Material>& _material) : shape(_shape), material(_material) {};

    bool intersect(const Ray& ray, Hit& res) const {
      if(shape->intersect(ray, res)) {
        res.hitPrimitive = this;
        return true;
      }
      else {
        return false;
      }
    };
};
#endif
