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
#endif
