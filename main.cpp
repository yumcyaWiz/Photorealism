#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "image.h"


int main() {
  Image img(512, 512);
  for(int i = 0; i < img.width; i++) {
    for(int j = 0; j < img.height; j++) {
      img.setPixel(i, j, RGB(i/255.0, j/255.0, 1));
    }
  }
  img.ppm_output("output.ppm");
}
