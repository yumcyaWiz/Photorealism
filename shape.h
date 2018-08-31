#ifndef SHAPE_H
#define SHAPE_H
#include "vec3.h"
#include "ray.h"
#include "hit.h"
#include "sampler.h"
class Shape {
  public:
    virtual bool intersect(const Ray& ray, Hit& res) const = 0;
    virtual float Pdf() const = 0;
    virtual Vec3 sample(const Hit& res, Sampler& sampler, Vec3& normal, float& pdf) const = 0;
};
#endif
