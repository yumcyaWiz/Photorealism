#ifndef IMAGE_H
#define IMAGE_H
#include <iostream>
#include <fstream>
#include "vec3.h"
#include "util.h"
class Image {
  public:
    int width;
    int height;
    RGB* data;

    Image(int _width, int _height) : width(_width), height(_height) {
      data = new RGB[width*height];
    };
    ~Image() {
      delete[] data;
    };

    RGB getPixel(int i, int j) const {
      if(i < 0 || i >= width || j < 0 || j >= height) {
        std::cerr << "Invalid Indexes" << std::endl;
        std::exit(1);
      }
      return data[i + width*j];
    };
    void setPixel(int i, int j, const RGB& c) {
      if(i < 0 || i >= width || j < 0 || j >= height) {
        std::cerr << "Invalid Indexes" << std::endl;
        std::exit(1);
      }
      data[i + width*j] = c;
    };

    void ppm_output(const std::string& filename) const {
      std::ofstream file(filename);
      
      file << "P3" << std::endl;
      file << width << " " << height << std::endl;
      file << "255" << std::endl;

      for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
          RGB c = this->getPixel(i, j);
          int r = clamp((int)(255*c.x), 0, 255);
          int g = clamp((int)(255*c.y), 0, 255);
          int b = clamp((int)(255*c.z), 0, 255);
          file << r << " " << g << " " << b << std::endl;
        }
      }
      file.close();
    };

    void divide(int k) {
      for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
          this->setPixel(i, j, this->getPixel(i, j)/k);
        }
      }
    };

    void gamma_correction() {
      for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
          this->setPixel(i, j, pow(this->getPixel(i, j), 1/2.2));
        }
      }
    };
};
#endif
