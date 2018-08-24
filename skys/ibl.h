#ifndef IBL_H
#define IBL_H
#include <string>
#include "../sky.h"
#include "../stb/stb_image.h"
class IBL : public Sky {
  public:
    int width;
    int height;
    RGB* data;

    IBL(const std::string& filename) {
      int n;
      data = stbi_loadf(filename.c_str(), &width, &height, &n, 0);
    };
    ~IBL() {
      stbi_image_free(&data);
    };

    RGB getColor(const Ray& ray) const {
      double phi = std::atan2(ray.direction.z, ray.direction.x);
      if(phi < 0) phi += 2*M_PI;
      float theta = std::acos(ray.direction.y);
      
      double u = phi/(2*M_PI);
      double v = theta/M_PI;
      
      int w = (int)(u * width);
      int h = (int)(v * height);
      int adr = 3*w + 3*width*h;
      return RGB(data[adr], data[adr + 1], data[adr + 2]);
    };
};
#endif
