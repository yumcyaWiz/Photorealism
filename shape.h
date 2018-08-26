#ifndef SHAPE_H
#define SHAPE_H
#include "vec3.h"
#include "ray.h"
#include "hit.h"
#include "sampler.h"
class Shape {
  public:
    virtual bool intersect(const Ray& ray, Hit& res) const = 0;
    virtual Vec3 sample(Sampler& sampler, Vec3& normal, double& pdf) const = 0;
};
#endif
