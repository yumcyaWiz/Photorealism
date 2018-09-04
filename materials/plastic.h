#ifndef PLASTIC_H
#define PLASTIC_H
#include "../material.h"
#include "../sampler.h"
class Plastic : public Material {
  public:
    Vec3 reflectance;
    float ior;

    Plastic(const RGB& _reflectance, float _ior) : Material(MATERIAL_TYPE::SPECULAR), reflectance(_reflectance), ior(_ior) {};

    RGB f(const Vec3& wo, const Vec3& wi) const {
      return RGB(0);
    };

    float Pdf(const Vec3& wo, const Vec3& wi) const {
      return 0;
    };

    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      float fr = fresnel(wo, Vec3(0, 1, 0), 1.0f, ior);
      if(sampler.getNext() < fr) {
        pdf = fr;
        wi = reflect(wo, Vec3(0, 1, 0));
        return fr*RGB(1);
      }
      else {
        pdf = 1 - fr;
        Vec2 u = sampler.getNext2D();
        wi = sampleCosineHemisphere(u);
        pdf = (1 - fr)*absCosTheta(wi)/M_PI + 0.001f;
        return (1 - fr)*reflectance/M_PI;
      }
    };
};
#endif
