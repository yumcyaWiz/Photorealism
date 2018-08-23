#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "image.h"
#include "hit.h"
#include "shapes/sphere.h"
#include "cameras/pinholeCamera.h"
#include "materials/lambert.h"


int main() {
  Image img(512, 512);
  PinholeCamera cam(Vec3(), Vec3(0, 0, 1));
  Sphere sphere(Vec3(0, 0, 3), 1.0);

  for(int i = 0; i < img.width; i++) {
    for(int j = 0; j < img.height; j++) {
      double u = (2.0*i - img.width)/img.width;
      double v = (2.0*j - img.height)/img.width;

      Ray ray;
      if(!cam.getRay(u, v, ray)) {
        img.setPixel(i, j, Vec3(0, 0, 0));
        continue;
      }

      Hit res;
      if(sphere.intersect(ray, res)) {
        img.setPixel(i, j, (res.hitNormal + 1)/2);
      }
      else {
        img.setPixel(i, j, Vec3(0, 0, 0));
      }
    }
  }
  img.ppm_output("output.ppm");
}
