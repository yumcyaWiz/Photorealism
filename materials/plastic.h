#ifndef PLASTIC_H
#define PLASTIC_H
#include "../material.h"
#include "../sampler.h"
class Plastic : public Material {
  public:
    std::shared_ptr<Texture> reflectance;
    float ior;

    Plastic(const std::shared_ptr<Texture>& _reflectance, float _ior) : Material(MATERIAL_TYPE::SPECULAR), reflectance(_reflectance), ior(_ior) {};

    RGB f(const Hit& res, const Vec3& wo, const Vec3& wi) const {
      float fr = fresnel(wo, Vec3(0, 1, 0), 1.0f, ior);
      return (1 - fr)*reflectance->getColor(res)/M_PI;
    };

    float Pdf(const Vec3& wo, const Vec3& wi) const {
      float fr = fresnel(wo, Vec3(0, 1, 0), 1.0f, ior);
      return (1 - fr)*absCosTheta(wi)/M_PI;
    };

    RGB sample(const Hit& res, const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      float fr = fresnel(wo, Vec3(0, 1, 0), 1.0f, ior);
      if(sampler.getNext() < fr) {
        pdf = fr;
        wi = reflect(wo, Vec3(0, 1, 0));
        return fr*RGB(1);
      }
      else {
        Vec2 u = sampler.getNext2D();
        wi = sampleCosineHemisphere(u);
        pdf = (1 - fr)*absCosTheta(wi)/M_PI + 0.001f;
        return (1 - fr)*reflectance->getColor(res)/M_PI;
      }
    };
};
#endif