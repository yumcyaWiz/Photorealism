#ifndef MIRROR_H
#define MIRROR_H
#include "../material.h"
#include "../sampler.h"
class Mirror : public Material {
  public:
    Vec3 reflectance;

    Mirror(const RGB& _reflectance) : Material(MATERIAL_TYPE::SPECULAR), reflectance(_reflectance) {};

    RGB f(const Vec3& wo, const Vec3& wi) const {
      return RGB(0);
    };

    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      pdf = 1.0;
      wi = reflect(wo, Vec3(0, 1, 0));
      return 1.0/absCosTheta(wi)*reflectance;
    };
};
#endif
