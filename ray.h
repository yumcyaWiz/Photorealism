#ifndef RAY_H
#define RAY_H
#include "vec3.h"
class Ray {
  public:
    Vec3 origin;
    Vec3 direction;
    constexpr static float tmin = 0.01;
    mutable float tmax;
    int hitCount;

    Ray() {
      hitCount = 0;
      tmax = 10000;
    };
    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {
      hitCount = 0;
      tmax = 10000;
    };

    Vec3 operator()(float t) const {
      return origin + t*direction;
    };
};
#endif
