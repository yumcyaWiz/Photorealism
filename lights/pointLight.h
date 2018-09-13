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
      return RGB(0);
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

    RGB sample_Le(Sampler& sampler, Ray& wo, Vec3& normal, float& pdf_pos, float& pdf_dir) const {
      wo = Ray(lightPos, sampleSphere(sampler.getNext2D()));
      normal = wo.direction;
      pdf_pos = 1.0f;
      pdf_dir = 1/(4*M_PI);
      return this->power;
    };
    void Pdf_Le(const Ray& ray, const Vec3& normal, float& pdf_pos, float& pdf_dir) const {
      pdf_pos = 0.0f;
      pdf_dir = 1/(4*M_PI);
    };
};
#endif
