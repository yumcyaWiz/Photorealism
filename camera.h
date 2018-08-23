#ifndef CAMERA_H
#define CAMERA_H
#include "vec3.h"
class Camera {
  public:
    Vec3 camPos;
    Vec3 camForward;
    Vec3 camRight;
    Vec3 camUp;

    Camera(const Vec3& _camPos, const Vec3& _camForward) : camPos(_camPos), camForward(_camForward) {
      orthonormalBasis(camForward, camRight, camUp);
    };

    virtual bool getRay(double u, double v, Ray& ray) const = 0;
};
#endif
