#ifndef GGX_H
#define GGX_H
#include "../material.h"
class GGX : public Material {
  public:
    RGB reflectance;
    float alpha;
    float ior;

    GGX(const RGB& _reflectance, float roughness, float _ior) : Material(MATERIAL_TYPE::GLOSSY), reflectance(_reflectance), ior(_ior) {
      alpha = roughness*roughness;
    };

    float D(const Vec3& wh) const {
      float tan2 = tan2Theta(wh);
      if(std::isinf(tan2)) return 0;

      float cos4 = cos2Theta(wh)*cos2Theta(wh);
      float A = 1 + tan2/(alpha*alpha);
      return 1/(M_PI*alpha*alpha*cos4*A*A);
    };
    float lambda(const Vec3& w) const {
      float tan = std::abs(tanTheta(w));
      if(std::isinf(tan)) return 0;
      return (-1 + std::sqrt(1 + alpha*tan*alpha*tan))/2;
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

      float fr = fresnel(wi, wh, 1.0f, ior);
      return reflectance * D(wh)*G(wo, wi)*fr / (4*cosThetaO*cosThetaI);
    };


    float Pdf(const Vec3& wo, const Vec3& wi) const {
      Vec3 wh = normalize(wo + wi);
      float pdf_wh = D(wh)*absCosTheta(wh);
      return pdf_wh/(4*dot(wo, wh));
    };


    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      Vec2 u = sampler.getNext2D();

      float tan2 = alpha*alpha*u.x/std::max(1.0f - u.x, 0.0f);
      float phi = 2*M_PI*u.y;

      float cos = 1/std::sqrt(1 + tan2);
      float sin = std::sqrt(std::max(1.0f - cos*cos, 0.0f));
      Vec3 wh = Vec3(std::cos(phi)*sin, cos, std::sin(phi)*sin);
      if(wo.y*wh.y < 0) return RGB(0);

      wi = reflect(wo, wh);
      if(wo.y*wi.y < 0) return RGB(0);

      pdf = Pdf(wo, wi);
      return f(wo, wi);
    };
};
#endif
