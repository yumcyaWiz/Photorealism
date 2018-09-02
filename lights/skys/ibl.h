#ifndef IBL_H
#define IBL_H
#include <cmath>
#include <string>
#include "../sky.h"
#include "../../sampler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../..//stb/stb_image.h"

class IBL : public Sky {
  public:
    int width;
    int height;
    RGB* data;
    RGB* data_sample;
    float intensity;
    float offsetX;
    float offsetY;

    IBL(const std::string& filename, float _intensity, float _offsetX, float _offsetY) : intensity(_intensity), offsetX(_offsetX), offsetY(_offsetY) {
      int n;
      float* img = stbi_loadf(filename.c_str(), &width, &height, &n, 0);

      data = new RGB[width*height];
      data_sample = new RGB[width*height];
      int index = 0;
      for(int i = 0; i < 3*width*height; i += 3) {
        data[index] = RGB(img[i], img[i + 1], img[i + 2]);
        int h = i / (3 * width);
        float theta = std::fmod((float)h/height * M_PI + offsetY, M_PI);
        data_sample[index] = std::sin(theta)*data[index];
        index++;
      }
      stbi_image_free(img);
    };
    ~IBL() {
      delete[] data;
      delete[] data_sample;
    };

    RGB Le(const Hit& res, const Ray& ray) const {
      float phi = std::atan2(ray.direction.z, ray.direction.x);
      if(phi < 0) phi += 2*M_PI;
      float theta = std::acos(ray.direction.y);
      
      float u = std::fmod(phi + offsetX, 2*M_PI)/(2*M_PI);
      float v = std::fmod(theta + offsetY, M_PI)/M_PI;
      
      int w = (int)(u * width);
      int h = (int)(v * height);
      return data[w + width*h];
    };
    float Pdf(const Hit& res, const Vec3& wi, const Hit& shadow_res) const {
      return 1/(4*M_PI);
    };
    RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, Vec3& samplePos, float& pdf) const {
      wi = sampleSphere(sampler.getNext2D());
      pdf = 1/(4*M_PI);
      return Le(res, Ray(Vec3(), wi));
    };
};
#endif
