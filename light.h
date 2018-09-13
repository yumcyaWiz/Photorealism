#ifndef LIGHT_H
#define LIGHT_H
#include "vec3.h"
#include "hit.h"

enum class LIGHT_TYPE {
  POINT,
  AREA,
  SKY
};


class Light {
  public:
    RGB power;
    LIGHT_TYPE type;

    Light(const LIGHT_TYPE& _type) : type(_type) {};
    Light(const RGB& _power, const LIGHT_TYPE& _type) : power(_power), type(_type) {};

    virtual RGB Le(const Hit& res, const Ray& ray) const = 0;
    virtual float Pdf(const Hit& res, const Vec3& wi, const Hit& shadow_res) const = 0;
    virtual RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, Vec3& samplePos, float& pdf) const = 0;

    virtual RGB sample_Le(Sampler& sampler, Ray& wo, Vec3& normal, float& pdf_pos, float& pdf_dir) const = 0;
    virtual void Pdf_Le(const Ray& ray, const Vec3& normal, float& pdf_pos, float& pdf_dir) const = 0;
};
#endif
