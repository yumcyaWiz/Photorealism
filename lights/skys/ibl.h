#ifndef IBL_H
#define IBL_H
#include <cmath>
#include <string>
#include <memory>
#include "../sky.h"
#include "../../sampler.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../..//stb/stb_image.h"
#endif

class IBL : public Sky {
  public:
    int width;
    int height;
    RGB* data;
    RGB* data_sample;
    float intensity;
    float offsetX;
    float offsetY;
    std::unique_ptr<Distribution2D> dist;


    IBL(const std::string& filename, float _intensity, float _offsetX, float _offsetY) : intensity(_intensity), offsetX(_offsetX), offsetY(_offsetY) {
      int n;
      float* img = stbi_loadf(filename.c_str(), &width, &height, &n, 0);

      data = new RGB[width*height];
      data_sample = new RGB[width*height];
      int index = 0;
      for(int i = 0; i < 3*width*height; i += 3) {
        data[index] = RGB(img[i], img[i + 1], img[i + 2]);
        int h = i / (3 * width);
        float theta = std::fmod(float(h)/height * M_PI + offsetY, M_PI);
        data_sample[index] = std::sin(theta)*data[index];
        index++;
      }
      stbi_image_free(img);

      float* f = new float[width*height];
      for(int i = 0; i < width*height; i++) {
        f[i] = data_sample[i].length();
      }
      dist.reset(new Distribution2D(f, width, height));
      delete[] f;

      //dist->ppm_output_pdf("ibl_pdf.ppm");
      //dist->ppm_output_cdf("ibl_cdf.ppm");
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
      return intensity * data[w + width*h];
    };


    float Pdf(const Hit& res, const Vec3& wi, const Hit& shadow_res) const {
      float phi = std::atan2(wi.z, wi.x);
      if(phi < 0) phi += 2*M_PI;
      float theta = std::acos(wi.y);
      if(std::sin(theta) == 0) return 0;

      float u = std::fmod(phi + offsetX, 2*M_PI)/(2*M_PI);
      float v = std::fmod(theta + offsetY, M_PI)/M_PI;

      return dist->Pdf(Vec2(u, v))/(2*M_PI*M_PI*std::sin(theta));
    };


    RGB sample(const Hit& res, Sampler& sampler, Vec3& wi, Vec3& samplePos, float& pdf) const {
      float pdf_p;
      Vec2 p = dist->sample(sampler.getNext2D(), pdf_p);

      float phi = std::fmod(p.x * 2*M_PI + offsetX, 2*M_PI);
      float theta = std::fmod(p.y * M_PI + offsetY, M_PI);
      if(std::sin(theta) == 0) return RGB(0);

      wi = Vec3(std::cos(phi)*std::sin(theta), std::cos(theta), std::sin(phi)*std::sin(theta));
      pdf = pdf_p/(2*M_PI*M_PI*std::sin(theta));

      int w = (int)(p.x*width);
      int h = (int)(p.y*height);
      if(w == width) w--;
      if(h == height) h--;
      return intensity * data[w + width*h];
    };
};
#endif
