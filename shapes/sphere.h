#ifndef SPHERE_H
#define SPHERE_H
#include "../vec3.h"
#include "../ray.h"
#include "../hit.h"
#include "../shape.h"

class Sphere : public Shape {
  public:
    Vec3 center;
    double radius;

    Sphere(const Vec3& _center, double _radius) : center(_center), radius(_radius) {};

    bool intersect(const Ray& ray, Hit& res) const {
      double a = ray.direction.length2();
      double b = 2*dot(ray.direction, ray.origin - center);
      double c = (ray.origin - center).length2() - radius*radius;
      double D = b*b - 4*a*c;
      if(D < 0) return false;

      double t0 = (-b - std::sqrt(D))/(2*a);
      double t1 = (-b + std::sqrt(D))/(2*a);

      double t = t0;
      if(t < 0) {
          t = t1;
          if(t < 0) return false;
      }

      res.t = t;
      res.hitPos = ray(t);
      res.hitNormal = normalize(res.hitPos - center);

      return true;
    };
};
#endif
