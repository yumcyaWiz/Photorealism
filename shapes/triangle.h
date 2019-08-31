#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "../vec2.h"
#include "../vec3.h"
#include "../ray.h"
#include "../hit.h"
#include "../shape.h"
class Triangle : public Shape {
  public:
    Vec3 p1, p2, p3;
    Vec3 n1, n2, n3;
    Vec2 t1, t2, t3;
    Vec3 face_normal;
    bool vertex_normal = false;
    bool vertex_uv = false;

    Triangle(const Vec3& _p1, const Vec3& _p2, const Vec3& _p3) : p1(_p1), p2(_p2), p3(_p3) {
      Vec3 a = normalize(p2 - p1);
      Vec3 b = normalize(p3 - p1);
      face_normal = normalize(cross(a, b));
    };
    Triangle(const Vec3& _p1, const Vec3& _p2, const Vec3& _p3, const Vec3& _n1, const Vec3& _n2, const Vec3& _n3) : p1(_p1), p2(_p2), p3(_p3), n1(_n1), n2(_n2), n3(_n3) {
      vertex_normal = true;
    };
    Triangle(const Vec3& _p1, const Vec3& _p2, const Vec3& _p3, const Vec2& _t1, const Vec2& _t2, const Vec2& _t3) : p1(_p1), p2(_p2), p3(_p3), t1(_t1), t2(_t2), t3(_t3) {
      Vec3 a = normalize(p2 - p1);
      Vec3 b = normalize(p3 - p1);
      face_normal = normalize(cross(a, b));
      vertex_uv = true;
    };
    Triangle(const Vec3& _p1, const Vec3& _p2, const Vec3& _p3, const Vec3& _n1, const Vec3& _n2, const Vec3& _n3, const Vec2& _t1, const Vec2& _t2, const Vec2& _t3) : p1(_p1), p2(_p2), p3(_p3), n1(_n1), n2(_n2), n3(_n3), t1(_t1), t2(_t2), t3(_t3) {
      vertex_normal = true;
      vertex_uv = true;
    };

    bool intersect(const Ray& ray, Hit& res) const {
        const float eps = 0.0001f;
        const Vec3 edge1 = p2 - p1;
        const Vec3 edge2 = p3 - p1;
        const Vec3 h = cross(ray.direction, edge2);
        const float a = dot(edge1, h);
        if(a >= -eps && a <= eps)
            return false;
        const float f = 1.0f/a;
        const Vec3 s = ray.origin - p1;
        const float u = f*dot(s, h);
        if(u < 0.0f || u > 1.0f)
            return false;
        const Vec3 q = cross(s, edge1);
        const float v = f*dot(ray.direction, q);
        if(v < 0.0f || u + v > 1.0f)
            return false;
        float t = f*dot(edge2, q);
        if(t <= ray.tmin || t > ray.tmax)
            return false;
        
        res.t = t;
        res.hitPos = ray(t);
        if(vertex_normal) {
            res.hitNormal = normalize((1.0f - u - v)*n1 + u*n2 + v*n3);
        }
        else {
            res.hitNormal = face_normal;
        }
        
        if(vertex_uv) {
          Vec2 uv = (1.0f - u - v)*t1 + u*t2 + v*t3;
          res.u = uv.x;
          res.v = uv.y;
        }
        else {
          res.u = u;
          res.v = v;
        }
        return true;
    };

    AABB worldBound() const {
      return AABB(min(p1, min(p2, p3)), max(p1, max(p2, p3)));
    };

    float surfaceArea() const {
      return 0.5f * std::abs(cross(p2 - p1, p3 - p1).length());
    };

    float Pdf() const {
      return 1/surfaceArea();
    };

    Vec3 sample(const Hit& res, Sampler& sampler, Vec3& normal, float &pdf) const {
        Vec2 u = sampleTriangle(sampler.getNext2D());
        Vec3 samplePos = (1.0f - u.x - u.y)*p1 + u.x*p2 + u.y*p3;
        if(vertex_normal)
            normal = normalize((1.0f - u.x - u.y)*n1 + u.x*n2 + u.y*n3);
        else
            normal = face_normal;
        pdf = 1.0f;
        return samplePos;
    };
    Vec3 sample2(Sampler& sampler, Vec3& normal, float& pdf) const {
      Hit res;
      return sample(res, sampler, normal, pdf);
    };
};
#endif
