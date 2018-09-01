#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include "../vec3.h"
#include "../hit.h"
#include "../light.h"
class PointLight : public Light {
  public:
    Vec3 lightPos;

    PointLight(const RGB& _power, const Vec3& _lightPos) : Light(_power, LIGHT_TYPE::POINT), lightPos(_lightPos) {};

    RGB Le(const Hit& res, const Ray& ray) const {
      return this->power;
    };

    float Pdf(const Hit& res, const Vec3& wi, const Hit& shadow_res) const {
      return 0;
    };

    RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, Vec3& samplePos, float& pdf) const {
      samplePos = lightPos;
      float dist2 = (samplePos - res.hitPos).length2();
      wi = normalize(lightPos - res.hitPos);
      pdf = 1.0f * dist2;
      return this->power;
    };
};
#endif
