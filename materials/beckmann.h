#ifndef BECKMANN_H
#define BECKMANN_H
#include "../material.h"
class Beckmann : public Material {
  public:
    std::shared_ptr<Texture> reflectance;
    float alpha;
    float ior;

    Beckmann(const std::shared_ptr<Texture>& _reflectance, float _roughness, float _ior) : Material(MATERIAL_TYPE::GLOSSY), reflectance(_reflectance), ior(_ior) {
      alpha = _roughness*_roughness;
    }

    float D(const Vec3& wh) const {
      float tan2 = tan2Theta(wh);
      if(std::isinf(tan2)) return 0;

      float cos4 = cos2Theta(wh)*cos2Theta(wh);
      return std::exp(-tan2/(alpha*alpha))/(M_PI*alpha*alpha*cos4);
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


    RGB f(const Hit& res, const Vec3& wo, const Vec3& wi) const {
      float cosThetaO = absCosTheta(wo), cosThetaI = absCosTheta(wi);
      Vec3 wh = wo + wi;
      if(cosThetaO == 0 || cosThetaI == 0) return RGB(0);
      if(wh.x == 0 || wh.y == 0 || wh.z == 0) return RGB(0);
      wh = normalize(wh);

      float fr = fresnel(wi, wh, 1.0f, ior);
      return reflectance->getColor(res) * D(wh)*G(wo, wi)*fr / (4*cosThetaO*cosThetaI);
    };


    float Pdf(const Vec3& wo, const Vec3& wi) const {
      Vec3 wh = normalize(wo + wi);
      float pdf_wh = D(wh)*absCosTheta(wh);
      return pdf_wh/(4*dot(wo, wh));
    };


    RGB sample(const Hit& res, const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      Vec2 u = sampler.getNext2D();

      float tan2, phi;
      float log = std::log(u.x);
      if(std::isinf(log)) log = 0;
      tan2 = -alpha*alpha*log;
      phi = 2*M_PI*u.y;

      float cos = 1/std::sqrt(1 + tan2);
      float sin = std::sqrt(std::max(1.0f - cos*cos, 0.0f));
      Vec3 wh = Vec3(std::cos(phi)*sin, cos, std::sin(phi)*sin);
      if(wo.y*wh.y < 0) wh = -wh;

      wi = reflect(wo, wh);
      if(wo.y*wi.y < 0) return RGB(0);

      pdf = Pdf(wo, wi);
      return f(res, wo, wi);
    };
};
#endif
