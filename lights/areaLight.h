#ifndef AREALIGHT_H
#define AREALIGHT_H
#include <memory>
#include "../shape.h"
#include "../light.h"
class AreaLight : public Light {
  public:
    std::shared_ptr<Shape> shape;

    AreaLight(const RGB& _power, const std::shared_ptr<Shape>& _shape) : Light(_power, LIGHT_TYPE::AREA), shape(_shape) {};

    RGB Le(const Hit& res) const {
      return power;
    };

    RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, Vec3& samplePos, float& pdf) const {
      float point_pdf;
      Vec3 normal;
      samplePos = shape->sample(sampler, normal, point_pdf);

      float dist2 = (samplePos - res.hitPos).length2();
      wi = normalize(samplePos - res.hitPos);
      float cos = dot(-wi, normal);
      if(cos <= 0) return RGB(0);

      pdf = point_pdf * dist2/cos;

      return power;
    };
};
#endif
