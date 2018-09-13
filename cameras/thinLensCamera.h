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
      float cos = dot(camForward, normalize(focusPoint - this->camPos));
      b = cos*(focusPoint - this->camPos).length() - a;
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

    float We(const Ray& ray, Vec2& pFilm) const {
      float cos = dot(ray.direction, camForward);
      if(cos < 0) return 0;

      Vec3 p = ray(b/cos);
      Vec3 l = normalize(lensCenter - p);
      float cos2 = dot(-l, camForward);
      Vec3 pf = p + (a + b)/cos2 * l;
      pf = worldToLocal(pf, camUp, camRight, camForward);
      pFilm = Vec2(pf.x, pf.y);
      if(std::abs(pFilm.x) > 1 || std::abs(pFilm.y) > 1) return 0;

      return 1;
    };

    float Sample_Wi(const Hit& res, Sampler& sampler, Vec3& wi, float& pdf, Vec2& pFilm) const {
      Vec2 l = sampleDisk(sampler.getNext2D());
      Vec3 pLens = lensCenter + lensRadius*(l.x*this->camRight + l.y*this->camUp);
      wi = normalize(pLens - res.hitPos);

      float dist2 = (pLens - res.hitPos).length2();
      float cos = dot(camForward, -wi);
      if(cos < 0) return 0;
      pdf = dist2/(M_PI*lensRadius*lensRadius*cos);

      return We(Ray(pLens, -wi), pFilm);
    };
};
#endif
