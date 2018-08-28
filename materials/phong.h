#ifndef PHONG_H
#define PHONG_H
#include "../material.h"
class Phong : public Material {
  public:
    RGB reflectance;
    float kd;
    float alpha;

    Phong(const RGB& _reflectance, float _kd, float _alpha) : Material(MATERIAL_TYPE::GLOSSY), reflectance(_reflectance), kd(_kd), alpha(_alpha) {};

    RGB f(const Vec3& wo, const Vec3& wi) const {
      Vec3 wh = normalize(wo + wi);
      return (1.0f - kd)*RGB(1.0f)*(alpha + 2.0f)/(2.0f*M_PI) * std::pow(absCosTheta(wh), alpha);
    };

    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      Vec2 u = sampler.getNext2D();
      if(sampler.getNext() < kd) {
        wi = sampleCosineHemisphere(u);
        pdf = kd * absCosTheta(wi)/M_PI;
        return kd * reflectance/M_PI;
      }
      else {
        Vec3 wh = sampleNCosineHemisphere(u, alpha);
        if(dot(wo, wh) < 0.0f) return RGB(0);
        wi = reflect(wo, wh);
        if(wi.y < 0.0f) return RGB(0.0f);
        float pdf_wh = (alpha + 2.0f)/(2*M_PI) * std::pow(absCosTheta(wh), alpha);
        pdf = (1.0f - kd) * pdf_wh/(4.0f*std::abs(dot(wo, wh)));
        return this->f(wo, wi);
      }
    };
};
#endif
