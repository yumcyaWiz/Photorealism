#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "image.h"
#include "shapes/sphere.h"


int main() {
  Image img(512, 512);
  for(int i = 0; i < img.width; i++) {
    for(int j = 0; j < img.height; j++) {
      img.setPixel(i, j, RGB(i/(double)img.width, j/(double)img.height, 1));
    }
  }
  img.ppm_output("output.ppm");
}
