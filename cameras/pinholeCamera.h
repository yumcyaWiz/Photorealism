#ifndef PINHOLECAMERA_H
#define PINHOLECAMERA_H
#include "../camera.h"
#include "../vec3.h"
#include "../ray.h"
class PinholeCamera : public Camera {
  public:
    double d;

    PinholeCamera(const Vec3& _camPos, const Vec3& _camForward) : Camera(_camPos, _camForward) {
      d = 1;
    };

    bool getRay(double u, double v, Sampler& sampler, Ray& ray) const {
      u = -u;
      v = -v;
      ray = Ray(camPos, normalize(d*camForward + u*camRight + v*camUp));
      return true;
    };
};
#endif
