#ifndef PHONG_H
#define PHONG_H
#include "../material.h"
class Phong : public Material {
  public:
    std::shared_ptr<Texture> reflectance;
    float kd;
    float alpha;

    Phong(const std::shared_ptr<Texture>& _reflectance, float _kd, float _alpha) : Material(MATERIAL_TYPE::GLOSSY), reflectance(_reflectance), kd(_kd), alpha(_alpha) {};

    RGB f(const Hit& res, const Vec3& wo, const Vec3& wi) const {
      Vec3 wh = wo + wi;
      if(cosTheta(wi) == 0 || cosTheta(wi) == 0) return RGB(0);
      if(wh.x == 0 || wh.y == 0 || wh.z == 0) return RGB(0);
      wh = normalize(wh);
      return kd*reflectance->getColor(res) + (1.0f - kd)*RGB(1.0f)*(alpha + 2.0f)/(2.0f*M_PI) * std::pow(absCosTheta(wh), alpha);
    };

    float Pdf(const Vec3& wo, const Vec3& wi) const {
      Vec3 wh = normalize(wo + wi);
      float pdf_wh = (alpha + 2.0f)/(2*M_PI) * std::pow((double)absCosTheta(wh), (double)alpha);
      return kd*absCosTheta(wi) + (1.0f - kd) * pdf_wh/(4.0f*std::abs(dot(wo, wh)) + 0.001f) + 0.001f;
    };

    RGB sample(const Hit& res, const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      Vec2 u = sampler.getNext2D();
      if(sampler.getNext() < kd) {
        wi = sampleCosineHemisphere(u);
        pdf = kd * absCosTheta(wi)/M_PI;
        return kd * reflectance->getColor(res)/M_PI;
      }
      else {
        Vec3 wh = sampleNCosineHemisphere(u, alpha);
        wi = reflect(wo, wh);
        if(wi.y < 0.0f) return RGB(0.0f);
        pdf = Pdf(wo, wi);
        return this->f(res, wo, wi);
      }
    };
};
#endif
