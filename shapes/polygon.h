#ifndef POLYGON_H
#define POLYGON_H
#include "../shape.h"
#include "triangle.h"
#include "../accels/bvh.h"
class Polygon : public Shape {
  public:
    std::vector<std::shared_ptr<Triangle>> triangles;
    std::shared_ptr<Accel<Triangle>> accel;
    float surfaceArea;

    Polygon(const std::vector<std::shared_ptr<Triangle>>& _triangles) : triangles(_triangles) {
      accel = std::make_shared<BVH<Triangle>>(triangles);

      surfaceArea = 0;
      for(const auto& triangle : triangles) {
        surfaceArea += triangle->surfaceArea();
      }
    };

    bool intersect(const Ray& ray, Hit& res) const {
      return accel->intersect(ray, res);
    };

    AABB worldBound() const {
      return accel->worldBound();
    };

    float Pdf() const {
      return 1/surfaceArea;
    };

    Vec3 sample(const Hit& res, Sampler& sampler, Vec3& normal, float &pdf) const {
      int tri = std::floor(triangles.size()*sampler.getNext());
      if(tri == triangles.size()) tri--;
      Vec3 samplePos = triangles[tri]->sample(res, sampler, normal, pdf);
      pdf = Pdf();
      return samplePos;
    };
};
#endif
