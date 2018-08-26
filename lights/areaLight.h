#ifndef AREALIGHT_H
#define AREALIGHT_H
#include <memory>
#include "../shape.h"
#include "../light.h"
class AreaLight : public Light {
  public:
    std::shared_ptr<Shape> shape;

    AreaLight(const RGB& _power, const std::shared_ptr<Shape>& _shape) : Light(_power), shape(_shape) {};

    RGB Le(const Hit& res) const {
      return power;
    };

    RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, double& pdf) const {
      double point_pdf;
      Vec3 normal;
      Vec3 shapePos = shape->sample(sampler, normal, point_pdf);

      double dist2 = (shapePos - res.hitPos).length2();
      wi = normalize(shapePos - res.hitPos);
      double cos = std::max(dot(-wi, normal), 0.0);
      pdf = point_pdf * dist2/cos;

      return power;
    };
};
#endif
