#ifndef THINLENSCAMERA_H
#define THINLENSCAMERA_H
#include "../camera.h"
class ThinLensCamera : public Camera {
  public:
    double a;
    double b;
    double f;
    double Fnumber;
    Vec3 lensCenter;
    double lensRadius;

    ThinLensCamera(const Vec3& _camPos, const Vec3& _camForward, const Vec3& focusPoint, double _a, double _Fnumber) : Camera(_camPos, _camForward), a(_a), Fnumber(_Fnumber) {
      b = (focusPoint - this->camPos).length() - a;
      f = 1/(1/a + 1/b);
      lensRadius = 0.5 * f/Fnumber;
      lensCenter = this->camPos + a*this->camForward;
    };

    bool getRay(double u, double v, Sampler& sampler, Ray& ray) const {
      Vec3 sensorPos = this->camPos + u*this->camRight + v*this->camUp;
      Vec3 sensorToLensCenter = normalize(lensCenter - sensorPos);
      double cos = dot(this->camForward, sensorToLensCenter);
      Vec3 objectPos = sensorPos + (a + b)/cos * sensorToLensCenter;

      Vec2 l = sampleDisk(sampler.getNext2D());
      Vec3 lensPos = lensCenter + lensRadius*(l.x*this->camRight + l.y*this->camUp);

      ray = Ray(lensPos, normalize(objectPos - lensPos));
      return true;
    };
};
#endif
