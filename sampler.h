#ifndef SAMPLER_H
#define SAMPLER_H
#include <cmath>
#include "vec2.h"
#include "vec3.h"
class Sampler {
  public:
    Sampler() {};
    virtual float getNext() = 0;
    virtual Vec2 getNext2D() = 0;
};


inline Vec2 sampleDisk(const Vec2& u) {
    const float r = std::sqrt(u.x);
    const float theta = 2 * M_PI * u.y;
    return Vec2(r * std::cos(theta), r * std::sin(theta));
}


inline Vec3 sampleSphere(const Vec2& u) {
  const float z = 1 - 2*u.x;
  const float r = std::sqrt(std::max(0.0f, 1.0f - z*z));
  const float phi = 2*M_PI*u.y;
  return Vec3(r*std::cos(phi), z, r*std::sin(phi));
}


inline Vec3 sampleHemisphere(const Vec2& u) {
    const float z = u.x;
    const float r = std::sqrt(std::max(0.0f, 1.0f - z*z));
    const float phi = 2.0*M_PI*u.y;
    return Vec3(r*std::cos(phi), z, r*std::sin(phi));
}

inline Vec3 sampleCosineHemisphere(const Vec2& u) {
    const Vec2 d = sampleDisk(u);
    const float z = std::sqrt(std::max(0.0f, 1.0f - u.x));
    return Vec3(d.x, z, d.y);
}

inline Vec3 sampleNCosineHemisphere(const Vec2& u, float alpha) {
    const float theta = std::acos(std::pow(1.0f - u.x, 1.0f/(alpha + 1.0f)));
    const float phi = 2.0*M_PI*u.y;
    return Vec3(std::cos(phi)*std::sin(theta), std::cos(theta), std::sin(phi)*std::sin(theta));
}
#endif
