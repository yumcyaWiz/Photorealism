#ifndef GLASS_H
#define GLASS_H
#include "../material.h"
class Glass : public Material {
  public:
    double n;
    RGB reflectance;

    Glass(double _n, const RGB& _reflectance) : Material(MATERIAL_TYPE::SPECULAR), n(_n), reflectance(_reflectance) {};

    RGB f(const Vec3& wo, const Vec3& wi) const {
      return RGB(0);
    };

    RGB sample(const Vec3& wo, Sampler& sampler, Vec3& wi, double& pdf) const {
      bool isEntering = cosTheta(wo) > 0;
      double n1, n2;
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

      double fr = fresnel(wo, normal, n1, n2);
      if(sampler.getNext() < fr) {
        wi = reflect(wo, normal);
        pdf = fr;
        return fr * 1/dot(wi, normal) * reflectance;
      }
      else {
        if(refract(wo, wi, normal, n1, n2)) {
          pdf = 1 - fr;
          return std::pow(n2/n1, 2.0) * (1 - fr) * 1/dot(wi, -normal) * reflectance;
        }
        else {
          wi = reflect(wo, normal);
          pdf = 1 - fr;
          return (1 - fr) * 1/dot(wi, normal) * reflectance;
        }
      }
    };
};
#endif
