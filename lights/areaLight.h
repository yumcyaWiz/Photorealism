#ifndef AREALIGHT_H
#define AREALIGHT_H
#include <memory>
#include "../shape.h"
#include "../light.h"
class AreaLight : public Light {
  public:
    std::shared_ptr<Shape> shape;

    AreaLight(const RGB& _power, const std::shared_ptr<Shape>& _shape) : Light(_power, LIGHT_TYPE::AREA), shape(_shape) {};

    RGB Le(const Hit& res, const Ray& ray) const {
      return power;
    };

    float Pdf(const Hit& res, const Vec3& wi, const Hit& shadow_res) const {
      float dist2 = (shadow_res.hitPos - res.hitPos).length2();
      float cos = dot(-wi, shadow_res.hitNormal);
      float pdf_A = shape->Pdf();
      return pdf_A * dist2/cos;
    };

    RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, Vec3& samplePos, float& pdf) const {
      float point_pdf;
      Vec3 normal;
      samplePos = shape->sample(res, sampler, normal, point_pdf);

      float dist2 = (samplePos - res.hitPos).length2();
      wi = normalize(samplePos - res.hitPos);
      float cos = dot(-wi, normal);
      if(cos <= 0) return RGB(0);

      pdf = point_pdf * dist2/cos;

      return power;
    };

    RGB sample_Le(Sampler& sampler, Ray& wo, Vec3& normal, float& pdf_pos, float& pdf_dir) const {
      Vec3 samplePos = shape->sample2(sampler, normal, pdf_pos);

      Vec3 dir_local = sampleCosineHemisphere(sampler.getNext2D());
      pdf_dir = absCosTheta(dir_local)/M_PI;
      Vec3 s, t;
      orthonormalBasis(normal, s, t);
      Vec3 dir = localToWorld(dir_local, normal, s, t);
      wo = Ray(samplePos, dir);

      return power;
    };
    void Pdf_Le(const Ray& ray, const Vec3& normal, float& pdf_pos, float& pdf_dir) const {
      pdf_pos = shape->Pdf();
      pdf_dir = std::max(dot(ray.direction, normal)/M_PI, 0.0);
    };
};
#endif
