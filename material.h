#ifndef MATERIAL_H
#define MATERIAL_H
#include <cmath>
#include <memory>
#include "vec3.h"
#include "sampler.h"
#include "texture.h"
enum class MATERIAL_TYPE {
  DIFFUSE,
  SPECULAR,
  GLOSSY
};


class Material {
  public:
    MATERIAL_TYPE type;

    Material(const MATERIAL_TYPE& _type) : type(_type) {};

    virtual RGB f(const Vec3& wo, const Vec3& wi) const = 0;
    virtual float Pdf(const Vec3& wo, const Vec3& wi) const = 0;
    virtual RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const = 0;
};


inline float cosTheta(const Vec3& w) {
    return w.y;
}
inline float absCosTheta(const Vec3& w) {
    return std::abs(w.y);
}
inline float cos2Theta(const Vec3& w) {
    return w.y*w.y;
}
inline float sinTheta(const Vec3& w) {
    return std::sqrt(std::max(1.0f - cos2Theta(w), 0.0f));
}
inline float sin2Theta(const Vec3& w) {
    return 1.0f - cos2Theta(w);
}
inline float tanTheta(const Vec3& w) {
  return sinTheta(w)/cosTheta(w);
}
inline float tan2Theta(const Vec3& w) {
  return tanTheta(w)*tanTheta(w);
}


inline float cosPhi(const Vec3& w) {
  float sin = sinTheta(w);
  return (sin == 0) ? 1 : clamp(w.x/sin, -1.0f, 1.0f);
}
inline float sinPhi(const Vec3& w) {
  float sin = sinTheta(w);
  return (sin == 0) ? 0 : clamp(w.y/sin, -1.0f, 1.0f);
}
inline float cos2Phi(const Vec3& w) {
  return cosPhi(w)*cosPhi(w);
}
inline float sin2Phi(const Vec3& w) {
  return sinPhi(w)*sinPhi(w);
}


inline float fresnel(const Vec3& wo, const Vec3& n, float n1, float n2) {
  const float f0 = std::pow((n1 - n2)/(n1 + n2), 2.0f);
  return f0 + (1.0f - f0)*std::pow(1.0f - dot(wo, n), 5.0f);
}


inline Vec3 reflect(const Vec3& w, const Vec3& n) {
    return -w + 2.0f*dot(w, n)*n;
}
inline bool refract(const Vec3& wi, Vec3& wt, const Vec3& n, float ior1, float ior2) {
    float eta = ior1/ior2;
    float cosThetaI = dot(wi, n);
    float sin2ThetaI = std::max(0.0f, 1.0f - cosThetaI*cosThetaI);
    float sin2ThetaT = eta*eta*sin2ThetaI;
    if(sin2ThetaT >= 1.0f) return false;
    float cosThetaT = std::sqrt(1.0f - sin2ThetaT);
    wt = eta*(-wi) + (eta*cosThetaI - cosThetaT)*n;
    return true;
}
#endif
