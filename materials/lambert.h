#ifndef LAMBERT_H
#define LAMBERT_H
#include "../material.h"
#include "../sampler.h"
#include "../texture.h"
class Lambert : public Material {
  public:
    std::shared_ptr<Texture> reflectance;

    Lambert(const std::shared_ptr<Texture>& _reflectance) : Material(MATERIAL_TYPE::DIFFUSE), reflectance(_reflectance) {};

    RGB f(const Hit& res, const Vec3& wo, const Vec3& wi) const {
      return reflectance->getColor(res)/M_PI;
    };

    float Pdf(const Vec3& wo, const Vec3& wi) const {
      return absCosTheta(wi)/M_PI + 0.001f;
    };

    RGB sample(const Hit& res, const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      Vec2 u = sampler.getNext2D();
      wi = sampleCosineHemisphere(u);
      pdf = Pdf(wo, wi);
      return f(res, wo, wi);
    };
};
#endif
