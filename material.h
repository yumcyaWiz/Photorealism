#ifndef MATERIAL_H
#define MATERIAL_H
#include <cmath>
#include <memory>
#include "vec3.h"
#include "sampler.h"
class Material {
  public:
    virtual RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, double& pdf) const = 0;
};


inline double cosTheta(const Vec3& w) {
    return w.y;
}
inline double absCosTheta(const Vec3& w) {
    return std::abs(w.y);
}
inline double cos2Theta(const Vec3& w) {
    return w.y*w.y;
}
inline double sinTheta(const Vec3& w) {
    return std::sqrt(1.0 - cos2Theta(w));
}
inline double sin2Theta(const Vec3& w) {
    return 1.0 - cos2Theta(w);
}


inline double fresnel(const Vec3& wo, const Vec3& n, double n1, double n2) {
  const double f0 = std::pow((n1 - n2)/(n1 + n2), 2.0);
  return f0 + (1 - f0)*std::pow(1 - dot(wo, n), 5.0);
}


inline Vec3 reflect(const Vec3& w, const Vec3& n) {
    return -w + 2.0*dot(w, n)*n;
}
inline bool refract(const Vec3& wi, Vec3& wt, const Vec3& n, double ior1, double ior2) {
    double eta = ior1/ior2;
    double cosThetaI = dot(wi, n);
    double sin2ThetaI = std::max(0.0, 1.0 - cosThetaI*cosThetaI);
    double sin2ThetaT = eta*eta*sin2ThetaI;
    if(sin2ThetaT >= 1.0) return false;
    double cosThetaT = std::sqrt(1.0 - sin2ThetaT);
    wt = eta*(-wi) + (eta*cosThetaI - cosThetaT)*n;
    return true;
}
#endif
