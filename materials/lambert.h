#ifndef LAMBERT_H
#define LAMBERT_H
#include "../material.h"
class Lambert : public Material {
  public:
    RGB reflectance;

    Lambert(const RGB& _reflectance) : reflectance(_reflectance) {};

    RGB brdf(const Vec3& wo, const Vec3& wi) const {
      return reflectance/M_PI;
    };
    RGB sample(const Vec3& wo, Vec3& wi, double& pdf) const {
      return reflectance/M_PI;
    };
};
#endif
