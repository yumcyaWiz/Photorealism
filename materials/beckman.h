#ifndef BECKMAN_H
#define BECKMAN_H
#include "../material.h"
class Beckman : public Material {
  public:
    RGB reflectance;
    float alpha;
    float n;

    Beckman(const RGB& _reflectance, float _roughness, float _n) : Material(MATERIAL_TYPE::GLOSSY), reflectance(_reflectance), n(_n) {
      alpha = _roughness*_roughness;
    }

    float D(const Vec3& wh) const {
      float tan = tan2Theta(wh);
      if(std::isinf(tan)) return 0;

      float cos4 = cos2Theta(wh)*cos2Theta(wh);
      return std::exp(-tan2Theta/(alpha*alpha))/(M_PI*alpha*alpha*cos4);
    };

    float lambda(const Vec3& w) const {
      float absTanTheta = std::abs(tanTheta(w));
      if(std::isinf(absTanTheta)) return 0;

      float a = 1/(alpha*absTanTheta);
      if(a >= 1.6f) return 0;
      else return (1 - 1.259f*a + 0.396f*a*a)/(3.535f*a + 2.181f*a*a);
    };
    float G1(const Vec3& w) const {
      return 1/(1 + lambda(w));
    };
    float G(const Vec3& wo, const Vec3& wi) const {
      return 1/(1 + lambda(wo) + lambda(wi));
    };


    RGB f(const Vec3& wo, const Vec3& wi) const {
      float cosThetaO = absCosTheta(wo), cosThetaI = absCosTheta(wi);
      Vec3 wh = wo + wi;
      if(cosThetaO == 0 || cosThetaI == 0) return RGB(0);
      if(wh.x == 0 || wh.y == 0 || wh.z == 0) return RGB(0);
      wh = normalize(wh);

      float fr = fresnel(wi, wh, 1.0f, n);
      return reflectance * D(wh)*G(wo, wi)*fr / (4*cosThetaO*cosThetaI);
    };
    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      Vec2 u = sampler.getNext2D();
      wi = sampleCosineHemisphere(u);
      pdf = absCosTheta(wi)/M_PI + 0.001f;
      return f(wo, wi);
    };
};
#endif
