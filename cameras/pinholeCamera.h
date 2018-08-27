#ifndef PINHOLECAMERA_H
#define PINHOLECAMERA_H
#include <memory>
#include "../camera.h"
#include "../vec3.h"
#include "../ray.h"
class PinholeCamera : public Camera {
  public:
    float d;

    PinholeCamera(const Vec3& _camPos, const Vec3& _camForward, const std::shared_ptr<Film>& _film) : Camera(_camPos, _camForward, _film) {
      d = 1;
    };

    bool getRay(float u, float v, Sampler& sampler, Ray& ray) const {
      u = -u;
      v = -v;
      ray = Ray(camPos, normalize(d*camForward + u*camRight + v*camUp));
      return true;
    };
};
#endif
