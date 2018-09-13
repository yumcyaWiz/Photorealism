#ifndef FILM_H
#define FILM_H
#include <iostream>
#include <fstream>
#include "vec3.h"
#include "util.h"
struct Pixel {
  RGB color;
  int n_sample;

  Pixel() : color(RGB(0)), n_sample(0) {};
  Pixel(const RGB& _color) : color(_color), n_sample(0) {};
};


class Film {
  public:
    int width;
    int height;
    Pixel* data;

    Film(int _width, int _height) : width(_width), height(_height) {
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
      return data[i + width*j].color/data[i + width*j].n_sample;
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

    void addSample(int i, int j, const RGB& L) const {
      if(i < 0 || i >= width || j < 0 || j >= height) {
        std::cerr << "Invalid Indexes" << std::endl;
        std::exit(1);
      }
      data[i + width*j].color += L;
      data[i + width*j].n_sample++;
    };
    void addSample(const Vec2& u, const RGB& L) const {
      int i = (u.x + 1.0f)/2.0f * width;
      int j = (u.y + 1.0f)/2.0f * height;
      if(i < 0 || i >= width || j < 0 || j >= height) {
        std::cerr << "Invalid Indexes" << std::endl;
        std::exit(1);
      }
      data[i + width*j].color += L;
      data[i + width*j].n_sample++;
    };
};
#endif
