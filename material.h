#ifndef MATERIAL_H
#define MATERIAL_H
#include "vec3.h"
class Material {
  public:
    virtual RGB brdf(const Vec3& wo, const Vec3& wi) const = 0;
    virtual RGB sample(const Vec3& wo, Vec3& wi, double& pdf) const = 0;
};
#endif
