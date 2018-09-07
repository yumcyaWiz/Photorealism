#ifndef GLASS_H
#define GLASS_H
#include "../material.h"
#include "../texture.h"
class Glass : public Material {
  public:
    std::shared_ptr<Texture> reflectance;
    float n;

    Glass(const std::shared_ptr<Texture>& _reflectance, float _n) : Material(MATERIAL_TYPE::SPECULAR), reflectance(_reflectance), n(_n) {};

    RGB f(const Hit& res, const Vec3& wo, const Vec3& wi) const {
      return RGB(0);
    };

    float Pdf(const Vec3& wo, const Vec3& wi) const {
      return 0;
    };

    RGB sample(const Hit& res, const Vec3& wo, Sampler& sampler, Vec3& wi, float& pdf) const {
      bool isEntering = cosTheta(wo) > 0;
      float n1, n2;
      Vec3 normal;
      if(isEntering) {
        n1 = 1.0;
        n2 = n;
        normal = Vec3(0, 1, 0);
      }
      else {
        n1 = n;
        n2 = 1.0;
        normal = Vec3(0, -1, 0);
      }

      float fr = fresnel(wo, normal, n1, n2);
      if(sampler.getNext() < fr) {
        wi = reflect(wo, normal);
        pdf = fr;
        return fr * 1/dot(wi, normal) * reflectance->getColor(res);
      }
      else {
        if(refract(wo, wi, normal, n1, n2)) {
          pdf = 1 - fr;
          return std::pow(n2/n1, 2.0) * (1 - fr) * 1/dot(wi, -normal) * reflectance->getColor(res);
        }
        else {
          wi = reflect(wo, normal);
          pdf = 1 - fr;
          return (1 - fr) * 1/dot(wi, normal) * reflectance->getColor(res);
        }
      }
    };
};
#endif
