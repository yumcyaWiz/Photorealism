#ifndef LIGHT_H
#define LIGHT_H
#include "vec3.h"
#include "hit.h"
class Light {
  public:
    RGB power;

    Light() {};
    Light(const RGB& _power) : power(_power) {};

    virtual RGB Le(const Hit& res) const = 0;
    virtual RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, double& pdf) const = 0;
};
#endif
