#ifndef LAMBERT_H
#define LAMBERT_H
#include "../material.h"
#include "../sampler.h"
class Lambert : public Material {
  public:
    RGB reflectance;

    Lambert(const RGB& _reflectance) : reflectance(_reflectance) {};

    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, double& pdf) const {
      Vec2 u = sampler.getNext2D();
      wi = sampleCosineHemisphere(u);
      pdf = absCosTheta(wi)/M_PI;
      return reflectance/M_PI;
    };
};
#endif
