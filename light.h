#ifndef LIGHT_H
#define LIGHT_H
#include "vec3.h"
#include "hit.h"

enum class LIGHT_TYPE {
  POINT,
  AREA
};


class Light {
  public:
    RGB power;
    LIGHT_TYPE type;

    Light() {};
    Light(const RGB& _power, const LIGHT_TYPE& _type) : power(_power), type(_type) {};

    virtual RGB Le(const Hit& res) const = 0;
    virtual RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, double& pdf) const = 0;
};
#endif
