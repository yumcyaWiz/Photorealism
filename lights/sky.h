#ifndef SKY_H
#define SKY_H
#include "../vec3.h"
#include "../ray.h"
#include "../light.h"
class Sky : public Light {
  public:
    Sky() : Light(LIGHT_TYPE::SKY) {};

    RGB sample_Le(Sampler& sampler, Ray& wo, Vec3& normal, float& pdf_pos, float& pdf_dir) const {
      return RGB(0);
    };
    void Pdf_Le(const Ray& ray, const Vec3& normal, float& pdf_pos, float& pdf_dir) const {};
};
#endif
