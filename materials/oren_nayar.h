#ifndef OREN_NAYAR_H
#define OREN_NAYAR_H
#include "../material.h"
class OrenNayar : public Material {
  public:
    RGB reflectance;
    float A;
    float B;

    OrenNayar(const RGB& _reflectance, float sigma) : Material(MATERIAL_TYPE::DIFFUSE), reflectance(_reflectance) {
      A = 1 - sigma*sigma/(2*(sigma*sigma + 0.33));
      B = 0.45*sigma*sigma/(sigma*sigma + 0.09);
    };

    RGB f(const Vec3& wo, const Vec3& wi) const {
      float sinThetaI = sinTheta(wi);
      float sinThetaO = sinTheta(wo);
      float maxCos = 0;
      if(sinThetaI > 1e-4 && sinThetaO > 1e-4) {
        float sinPhiI = sinPhi(wi), cosPhiI = cosPhi(wi);
        float sinPhiO = sinPhi(wo), cosPhiO = cosPhi(wo);
        maxCos = std::max(cosPhiI*cosPhiO + sinPhiI*sinPhiO, 0.0f);
      }

      float sinAlpha, tanBeta;
      if(absCosTheta(wi) > absCosTheta(wo)) {
        sinAlpha = sinThetaO;
        tanBeta = sinThetaI / absCosTheta(wi);
      }
      else {
        sinAlpha = sinThetaI;
        tanBeta = sinThetaO / absCosTheta(wo);
      }

      return reflectance/M_PI * (A + B * maxCos  * sinAlpha * tanBeta);
    };

    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      Vec2 u = sampler.getNext2D();
      wi = sampleCosineHemisphere(u);
      pdf = absCosTheta(wi)/M_PI + 0.001f;
      return this->f(wo, wi);
    };
};
#endif
