#ifndef SPHERE_H
#define SPHERE_H
#include "../vec3.h"
#include "../ray.h"
#include "../hit.h"
#include "../shape.h"

class Sphere : public Shape {
  public:
    Vec3 center;
    float radius;

    Sphere(const Vec3& _center, float _radius) : center(_center), radius(_radius) {};

    bool intersect(const Ray& ray, Hit& res) const {
      float a = ray.direction.length2();
      float b = 2*dot(ray.direction, ray.origin - center);
      float c = (ray.origin - center).length2() - radius*radius;
      float D = b*b - 4*a*c;
      if(D < 0) return false;

      float t0, t1;
      if(b > 0) {
        t0 = -2*c/(b + std::sqrt(D));
        t1 = (-b - std::sqrt(D))/(2*a);
      }
      else {
        t1 = (-b + std::sqrt(D))/(2*a);
        t0 = 2*c/(-b + std::sqrt(D));
      }

      float t = t0;
      if(t > ray.tmax) return false;
      if(t < ray.tmin) {
          t = t1;
          if(t < ray.tmin || t > ray.tmax) return false;
      }

      res.t = t;
      res.hitPos = ray(t);
      res.hitNormal = normalize(res.hitPos - center);

      float phi = std::atan2(res.hitNormal.z, res.hitNormal.x);
      if(phi < 0) phi += 2*M_PI;
      float theta = std::acos(res.hitNormal.y);
      res.u = phi/(2*M_PI);
      res.v = theta/M_PI;

      return true;
    };

    float Pdf() const {
      return 1/(2*M_PI*radius*radius);
    };

    Vec3 sample(const Hit& res, Sampler& sampler, Vec3& normal, float& pdf) const {
      pdf = Pdf();

      Vec3 n = normalize(res.hitPos - center);
      Vec3 s, t;
      orthonormalBasis(n, s, t);

      Vec2 u = sampler.getNext2D();
      Vec3 samplePos = center + radius*localToWorld(sampleHemisphere(u), n, s, t);
      normal = normalize(samplePos - center);
      return samplePos;
    };
    Vec3 sample2(Sampler& sampler, Vec3& normal, float& pdf) const {
      Vec3 samplePos = center + radius*sampleSphere(sampler.getNext2D());
      pdf = 1/(4*M_PI*radius*radius);
      return samplePos;
    };

    AABB worldBound() const {
      return AABB(center - Vec3(radius), center + Vec3(radius));
    };
};
#endif
