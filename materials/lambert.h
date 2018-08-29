#ifndef LAMBERT_H
#define LAMBERT_H
#include "../material.h"
#include "../sampler.h"
class Lambert : public Material {
  public:
    RGB reflectance;

    Lambert(const RGB& _reflectance) : Material(MATERIAL_TYPE::DIFFUSE), reflectance(_reflectance) {};

    RGB f(const Vec3& wo, const Vec3& wi) const {
      return reflectance/M_PI;
    };

    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      Vec2 u = sampler.getNext2D();
      wi = sampleCosineHemisphere(u);
      pdf = absCosTheta(wi)/M_PI + 0.001f;
      return reflectance/M_PI;
    };
};
#endif
