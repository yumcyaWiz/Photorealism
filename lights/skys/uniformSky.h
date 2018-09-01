#ifndef UNIFORMSKY_H
#define UNIFORMSKY_H
#include "../sky.h"
class UniformSky : public Sky {
  public:
    RGB color;

    UniformSky(const RGB& _color) : color(_color) {};

    RGB Le(const Hit& res, const Ray& ray) const {
      return color;
    };
    float Pdf(const Hit& res, const Vec3& wi, const Hit& shadow_res) const {
      return 1/(2*M_PI);
    };
    RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, Vec3& samplePos, float& pdf) const {
      Vec3 n = res.hitNormal;
      Vec3 s, t;
      orthonormalBasis(n, s, t);
      Vec3 p = sampleHemisphere(sampler.getNext2D());
      wi = localToWorld(p, n, s, t);
      pdf = 1/(2*M_PI);
      return color;
    };
};
#endif
