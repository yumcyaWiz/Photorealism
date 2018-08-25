#ifndef CAMERA_H
#define CAMERA_H
#include <memory>
#include "vec3.h"
#include "sampler.h"
#include "film.h"
class Camera {
  public:
    Vec3 camPos;
    Vec3 camForward;
    Vec3 camRight;
    Vec3 camUp;
    std::shared_ptr<Film> film;

    Camera(const Vec3& _camPos, const Vec3& _camForward, const std::shared_ptr<Film>& _film) : camPos(_camPos), camForward(_camForward), film(_film) {
      orthonormalBasis(camForward, camRight, camUp);
    };

    virtual bool getRay(double u, double v, Sampler& sampler, Ray& ray) const = 0;
};
#endif
