#ifndef SAMPLER_H
#define SAMPLER_H
#include <cmath>
#include "vec2.h"
#include "vec3.h"
class Sampler {
  public:
    Sampler() {};
    virtual double getNext() = 0;
    virtual Vec2 getNext2D() = 0;
};


inline Vec2 sampleDisk(const Vec2& u) {
    const double r = std::sqrt(u.x);
    const double theta = 2 * M_PI * u.y;
    return Vec2(r * std::cos(theta), r * std::sin(theta));
}

inline Vec3 sampleHemisphere(const Vec2& u) {
    const double z = u.x;
    const double r = std::sqrt(std::max(0.0, 1.0 - z*z));
    const double phi = 2.0*M_PI*u.y;
    return Vec3(r*std::cos(phi), z, r*std::sin(phi));
}

inline Vec3 sampleCosineHemisphere(const Vec2& u) {
    const Vec2 d = sampleDisk(u);
    const double z = std::sqrt(std::max(0.0, 1.0 - u.x));
    return Vec3(d.x, z, d.y);
}

inline Vec3 sampleNCosineHemisphere(const Vec2& u, double alpha) {
    const double theta = std::acos(std::pow(1.0 - u.x, 1.0/(alpha + 1.0)));
    const double phi = 2.0*M_PI*u.y;
    return Vec3(std::cos(phi)*std::sin(theta), std::cos(theta), std::sin(phi)*std::sin(theta));
}
#endif
