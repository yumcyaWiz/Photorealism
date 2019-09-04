#ifndef FILM_H
#define FILM_H
#include <iostream>
#include <fstream>

#include "tinypngout/TinyPngOut.hpp"

#include "vec3.h"
#include "util.h"
#include "filter.h"


struct Pixel {
  RGB color;
  int n_sample;
  float filter_accum;

  Pixel() : color(RGB(0)) {
    n_sample = 0;
    filter_accum = 0;
  };
  Pixel(const RGB& _color) : color(_color) {
    n_sample = 0;
    filter_accum = 0;
  };
};


class Film {
  public:
    int width;
    int height;
    Vec2 radius;
    Pixel* data;
    std::shared_ptr<Filter> filter;

    Film(int _width, int _height, const std::shared_ptr<Filter>& _filter) : width(_width), height(_height), filter(_filter) {
      data = new Pixel[width*height];
    };
    ~Film() {
      delete[] data;
    };

    RGB getPixel(int i, int j) const {
      if(i < 0 || i >= width || j < 0 || j >= height) {
        std::cerr << "Invalid Indexes" << std::endl;
        std::exit(1);
      }
      return data[i + width*j].color/data[i + width*j].filter_accum;
    };
    std::vector<RGB> getPixels() const {
      std::vector<RGB> pixels(width*height);
      for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
          pixels[i + width*j] = data[i + width*j].color;
        }
      }
      return pixels;
    };
    std::vector<uint8_t> getPixels_RGB() const {
      std::vector<uint8_t> pixels(3*width*height);
      for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
          for(int k = 0; k < 3; k++) {
            if(k == 0)
              pixels[k + 3*i + 3*width*j] = data[i + width*j].color.x;
            else if(k == 1)
              pixels[k + 3*i + 3*width*j] = data[i + width*j].color.y;
            else if(k == 2)
              pixels[k + 3*i + 3*width*j] = data[i + width*j].color.z;
          }
        }
      }
      return pixels;
    };
    void setPixel(int i, int j, const RGB& c) {
      if(i < 0 || i >= width || j < 0 || j >= height) {
        std::cerr << "Invalid Indexes" << std::endl;
        std::exit(1);
      }
      data[i + width*j] = Pixel(c);
    };

    void ppm_output(const std::string& filename) const {
      std::ofstream file(filename);
      
      file << "P3" << std::endl;
      file << width << " " << height << std::endl;
      file << "255" << std::endl;

      for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
          RGB c = pow(this->getPixel(i, j), 1/2.2);
          int r = clamp((int)(255*c.x), 0, 255);
          int g = clamp((int)(255*c.y), 0, 255);
          int b = clamp((int)(255*c.z), 0, 255);
          file << r << " " << g << " " << b << std::endl;
        }
      }
      file.close();
    };

    void png_output(const std::string& filename) const {
      try {
        std::ofstream file(filename);
        TinyPngOut png(static_cast<uint32_t>(width), static_cast<uint32_t>(height), file);
        png.write(this->getPixels_RGB().data(), width*height);
      }
      catch(const char *msg) {
        std::cerr << msg << std::endl;
      }
    };

    void addSample(int i, int j, const RGB& L) const {
      if(i < 0 || i >= width || j < 0 || j >= height) {
        std::cerr << "Invalid Indexes" << std::endl;
        std::exit(1);
      }
      data[i + width*j].color += L;
      data[i + width*j].filter_accum++;
      data[i + width*j].n_sample++;
    };
    void addSample(const Vec2& u, const RGB& L) const {
      float i = (u.x * height + width)/2.0f;
      float j = (u.y * height + height)/2.0f;
      float rx = filter->radius.x;
      float ry = filter->radius.y;
      for(int iy = j - ry; iy <= j + ry; iy++) {
        for(int ix = i - rx; ix <= i + rx; ix++) {
          if(ix < 0 || ix >= width || iy < 0 || iy >= height) continue;
          Vec2 pixelPos(ix + 0.5f, iy + 0.5f);
          Vec2 p = Vec2(i, j) - pixelPos;
          float f = filter->eval(p);
          data[ix + width*iy].color += f*L;
          data[ix + width*iy].n_sample++;
          data[ix + width*iy].filter_accum += f;
        }
      }
    };

    void divide(float denom) {
      for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
          data[i + width*j].color /= denom;
        }
      }
    };
};
#endif
