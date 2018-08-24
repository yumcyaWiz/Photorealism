#ifndef MIRROR_H
#define MIRROR_H
#include "../material.h"
#include "../sampler.h"
class Mirror : public Material {
  public:
    Vec3 reflectance;

    Mirror(const RGB& _reflectance) : reflectance(_reflectance) {
    };

    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, double& pdf) const {
      pdf = 1.0f;
      wi = reflect(wo, Vec3(0, 1, 0));
      return 1.0f/absCosTheta(wi)*reflectance;
    };
};
#endif
