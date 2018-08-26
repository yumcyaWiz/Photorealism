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
    std::shared_ptr<Light> light = nullptr;

    Primitive(const std::shared_ptr<Shape>& _shape, const std::shared_ptr<Material>& _material, const std::shared_ptr<Light>& _light = nullptr) : shape(_shape), material(_material), light(_light) {};

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
