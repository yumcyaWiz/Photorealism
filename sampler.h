#ifndef SAMPLER_H
#define SAMPLER_H
#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>
#include "vec2.h"
#include "vec3.h"
class Sampler {
  public:
    Sampler() {};
    virtual float getNext() = 0;
    virtual Vec2 getNext2D() = 0;
};


inline Vec2 sampleDisk(const Vec2& u) {
    const float r = std::sqrt(u.x);
    const float theta = 2 * M_PI * u.y;
    return Vec2(r * std::cos(theta), r * std::sin(theta));
}


inline Vec3 sampleSphere(const Vec2& u) {
  const float z = 1 - 2*u.x;
  const float r = std::sqrt(std::max(0.0f, 1.0f - z*z));
  const float phi = 2*M_PI*u.y;
  return Vec3(r*std::cos(phi), z, r*std::sin(phi));
}


inline Vec3 sampleHemisphere(const Vec2& u) {
    const float z = u.x;
    const float r = std::sqrt(std::max(0.0f, 1.0f - z*z));
    const float phi = 2.0*M_PI*u.y;
    return Vec3(r*std::cos(phi), z, r*std::sin(phi));
}

inline Vec3 sampleCosineHemisphere(const Vec2& u) {
    const Vec2 d = sampleDisk(u);
    const float z = std::sqrt(std::max(0.0f, 1.0f - u.x));
    return Vec3(d.x, z, d.y);
}

inline Vec3 sampleNCosineHemisphere(const Vec2& u, float alpha) {
    const float theta = std::acos(std::pow(std::max(1.0 - u.x, 0.0), 1.0/(alpha + 1.0)));
    const float phi = 2.0*M_PI*u.y;
    return Vec3(std::cos(phi)*std::sin(theta), std::cos(theta), std::sin(phi)*std::sin(theta));
}


class Distribution1D {
  public:
    int n;
    std::vector<float> func;
    std::vector<float> cdf;
    float funcInt;

    Distribution1D(const float* f, int _n) : n(_n), func(f, f + n), cdf(n + 1) {
      cdf[0] = 0;
      for(int i = 1; i <= n; i++) {
        cdf[i] = cdf[i - 1] + func[i - 1]/n;
      }
      funcInt = cdf[n];

      for(int i = 1; i <= n; i++) {
        cdf[i] /= funcInt;
      }
    };

    float sample(float u, float& pdf, int* offset = nullptr) const {
      int index = std::lower_bound(func.begin(), func.end(), u) - func.begin();
      if(offset) offset = &index;
      float du = u - func[index];
      du /= func[index + 1] - func[index];
      pdf = func[index]/funcInt;
      return (index + du)/n;
    };
};


class Distribution2D {
  public:
    int nx;
    int ny;
    std::vector<std::unique_ptr<Distribution1D>> px;
    std::unique_ptr<Distribution1D> py;

    Distribution2D(const float* f, int _nx, int _ny) : nx(_nx), ny(_ny) {
      for(int y = 0; y < ny; y++) {
        px.emplace_back(new Distribution1D(&f[y * nx], nx));
      }

      std::vector<float> marginal;
      for(int y = 0; y < ny; y++) {
        marginal.push_back(px[y]->funcInt);
      }
      py.reset(new Distribution1D(&marginal[0], ny));
    };

    Vec2 sample(const Vec2& u, float& pdf) const {
      float pdfx, pdfy;
      int y;
      float d0 = py->sample(u.y, pdfy, &y);
      float d1 = px[y]->sample(u.x, pdfx);
      pdf = pdfx * pdfy;
      return Vec2(d0, d1);
    };
};
#endif
