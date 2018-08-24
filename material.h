#ifndef MATERIAL_H
#define MATERIAL_H
#include <cmath>
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
