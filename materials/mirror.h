#ifndef MIRROR_H
#define MIRROR_H
#include "../material.h"
#include "../sampler.h"
#include "../texture.h"
class Mirror : public Material {
  public:
    std::shared_ptr<Texture> reflectance;

    Mirror(const std::shared_ptr<Texture>& _reflectance) : Material(MATERIAL_TYPE::SPECULAR), reflectance(_reflectance) {};

    RGB f(const Hit& res, const Vec3& wo, const Vec3& wi) const {
      return RGB(0);
    };

    float Pdf(const Vec3& wo, const Vec3& wi) const {
      return 0;
    };

    RGB sample(const Hit& res, const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      pdf = 1.0;
      wi = reflect(wo, Vec3(0, 1, 0));
      return 1.0/absCosTheta(wi)*reflectance->getColor(res);
    };
};
#endif
