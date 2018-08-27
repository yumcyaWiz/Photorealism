#ifndef IBL_H
#define IBL_H
#include <cmath>
#include <string>
#include "../sky.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

class IBL : public Sky {
  public:
    int width;
    int height;
    float* data;
    float intensity;
    float offsetX;
    float offsetY;

    IBL(const std::string& filename, float _intensity, float _offsetX, float _offsetY) : intensity(_intensity), offsetX(_offsetX), offsetY(_offsetY) {
      int n;
      data = stbi_loadf(filename.c_str(), &width, &height, &n, 0);
    };
    ~IBL() {
      stbi_image_free(data);
    };

    RGB getColor(const Ray& ray) const {
      float phi = std::atan2(ray.direction.z, ray.direction.x);
      if(phi < 0) phi += 2*M_PI;
      float theta = std::acos(ray.direction.y);
      
      float u = std::fmod(phi + offsetX, 2*M_PI)/(2*M_PI);
      float v = std::fmod(theta + offsetY, M_PI)/M_PI;
      
      int w = (int)(u * width);
      int h = (int)(v * height);
      int adr = 3*w + 3*width*h;
      return RGB(data[adr], data[adr + 1], data[adr + 2]);
    };
};
#endif
