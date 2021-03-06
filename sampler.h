#ifndef SAMPLER_H
#define SAMPLER_H
#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include "vec2.h"
#include "vec3.h"
#include "util.h"
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


inline Vec2 sampleTriangle(const Vec2& u) {
  float sqrt = std::sqrt(u.x);
  return Vec2(1 - sqrt, u.y*sqrt);
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

    float sample(float u, float& pdf, int& offset) const {
      int index = std::lower_bound(cdf.begin(), cdf.end(), u) - cdf.begin();
      if(index == n) index--;
      offset = index;
      float du = u - cdf[index];
      if(cdf[index + 1] - cdf[index] > 0) {
        du /= cdf[index + 1] - cdf[index];
      }
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
      int x;
      int y;
      float d1 = py->sample(u.y, pdfy, y);
      float d0 = px[y]->sample(u.x, pdfx, x);
      pdf = pdfx * pdfy;
      return Vec2(d0, d1);
    };

    float Pdf(const Vec2& u) const {
      int ix = clamp(int(u.x * px[0]->n), 0, px[0]->n - 1);
      int iy = clamp(int(u.y * py->n), 0, py->n - 1);
      return px[iy]->func[ix] / py->funcInt;
    };

    void ppm_output_pdf(const std::string& filename) const {
      std::ofstream file(filename);
      file << "P3" << std::endl;
      file << nx << " " << ny << std::endl;
      file << "255" << std::endl;
      for(int j = 0; j < ny; j++) {
        for(int i = 0; i < nx; i++) {
          float intensity = px[j]->func[i] / py->funcInt;
          int col = clamp((int)intensity, 0, 255);
          file << col << " " << col << " " << col << std::endl;
        }
      }
      file.close();
    };
    void ppm_output_cdf(const std::string& filename) const {
      std::ofstream file(filename);
      file << "P3" << std::endl;
      file << nx << " " << ny << std::endl;
      file << "255" << std::endl;
      for(int j = 0; j < ny; j++) {
        for(int i = 0; i < nx; i++) {
          float intensity = 255*px[j]->cdf[i];
          int col = clamp((int)intensity, 0, 255);
          file << col << " " << col << " " << col << std::endl;
        }
      }
      file.close();
    };
};
#endif
