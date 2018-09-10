#ifndef PLASTIC_H
#define PLASTIC_H
#include "../material.h"
#include "../sampler.h"
#include "../samplers/mt.h"
class Plastic : public Material {
  public:
    std::shared_ptr<Texture> reflectance;
    float ior;
    mutable Mt mt;

    Plastic(const std::shared_ptr<Texture>& _reflectance, float _ior) : Material(MATERIAL_TYPE::SPECULAR), reflectance(_reflectance), ior(_ior) {};

    RGB f(const Hit& res, const Vec3& wo, const Vec3& wi) const {
      float fr = fresnel(wo, Vec3(0, 1, 0), 1.0f, ior);
      if(mt.getNext() < fr) {
        return RGB(0);
      }
      else {
        return reflectance->getColor(res)/M_PI;
      }
    };

    float Pdf(const Vec3& wo, const Vec3& wi) const {
      float fr = fresnel(wo, Vec3(0, 1, 0), 1.0f, ior);
      if(mt.getNext() < fr) {
        return 0;
      }
      else {
        return absCosTheta(wi)/M_PI + 0.001f;
      }
    };

    RGB sample(const Hit& res, const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      float fr = fresnel(wo, Vec3(0, 1, 0), 1.0f, ior);
      if(sampler.getNext() < fr) {
        pdf = 1.0;
        wi = reflect(wo, Vec3(0, 1, 0));
        return RGB(0.9)/absCosTheta(wi);
      }
      else {
        Vec2 u = sampler.getNext2D();
        wi = sampleCosineHemisphere(u);
        pdf = absCosTheta(wi)/M_PI + 0.001f;
        return reflectance->getColor(res)/M_PI;
      }
    };
};
#endif
