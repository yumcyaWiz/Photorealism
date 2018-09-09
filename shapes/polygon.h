#ifndef POLYGON_H
#define POLYGON_H
#include "../shape.h"
#include "triangle.h"
#include "../accels/linear.h"
#include "../accels/bvh.h"
class Polygon : public Shape {
  public:
    std::vector<std::shared_ptr<Triangle>> triangles;
    std::shared_ptr<Accel<Triangle>> accel;
    float surfaceArea;
    std::string accel_type;

    Polygon(const std::vector<std::shared_ptr<Triangle>>& _triangles, const std::string& _accel_type) : triangles(_triangles), accel_type(_accel_type) {
      if(accel_type == "bvh") {
        accel = std::make_shared<BVH<Triangle>>(triangles, 4);
      }
      else if(accel_type == "linear") {
        accel = std::make_shared<Linear<Triangle>>(triangles);
      }
      else {
        std::cerr << "Invalid Accel type" << std::endl;
        std::exit(1);
      }

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
      return 1.0f/surfaceArea;
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
