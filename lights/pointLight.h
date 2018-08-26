#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include "../vec3.h"
#include "../hit.h"
#include "../light.h"
class PointLight : public Light {
  public:
    Vec3 lightPos;

    PointLight(const RGB& _power, const Vec3& _lightPos) : Light(_power, LIGHT_TYPE::POINT), lightPos(_lightPos) {};

    RGB Le(const Hit& res) const {
      return this->power;
    };

    RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, double& pdf) const {
      pdf = 1.0;
      wi = normalize(lightPos - res.hitPos);
      return this->power;
    };
};
#endif
