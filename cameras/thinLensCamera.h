#ifndef THINLENSCAMERA_H
#define THINLENSCAMERA_H
#include <memory>
#include "../camera.h"
class ThinLensCamera : public Camera {
  public:
    float a;
    float b;
    float f;
    float Fnumber;
    Vec3 lensCenter;
    float lensRadius;

    ThinLensCamera(const Vec3& _camPos, const Vec3& _camForward, const std::shared_ptr<Film>& _film, const Vec3& focusPoint, float _a, float _Fnumber) : Camera(_camPos, _camForward, _film), a(_a), Fnumber(_Fnumber) {
      b = (focusPoint - this->camPos).length() - a;
      f = 1/(1/a + 1/b);
      lensRadius = 0.5 * f/Fnumber;
      lensCenter = this->camPos + a*this->camForward;
    };

    bool getRay(float u, float v, Sampler& sampler, Ray& ray, float& weight) const {
      Vec3 sensorPos = this->camPos + u*this->camRight + v*this->camUp;
      Vec3 sensorToLensCenter = normalize(lensCenter - sensorPos);
      float cos = dot(this->camForward, sensorToLensCenter);
      Vec3 objectPos = sensorPos + (a + b)/cos * sensorToLensCenter;

      Vec2 l = sampleDisk(sampler.getNext2D());
      Vec3 lensPos = lensCenter + lensRadius*(l.x*this->camRight + l.y*this->camUp);

      ray = Ray(lensPos, normalize(objectPos - lensPos));
      weight = cos*cos / (lensPos - sensorPos).length2();
      return true;
    };
};
#endif
