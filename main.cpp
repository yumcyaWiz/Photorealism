#include <iostream>
#include <memory>
#include "vec3.h"
#include "ray.h"
#include "image.h"
#include "hit.h"
#include "shapes/sphere.h"
#include "cameras/pinholeCamera.h"
#include "materials/lambert.h"
#include "primitive.h"
#include "accels/linear.h"


int main() {
  Image img(512, 512);
  PinholeCamera cam(Vec3(), Vec3(0, 0, 1));
  Linear accel;

  auto sphere1 = std::make_shared<Sphere>(Vec3(0, 0, 3), 1);
  auto sphere2 = std::make_shared<Sphere>(Vec3(0, -10001, 0), 10000);

  auto mat = std::make_shared<Lambert>(RGB(1, 1, 1));

  auto prim1 = std::make_shared<Primitive>(sphere1, mat);
  auto prim2 = std::make_shared<Primitive>(sphere2, mat);

  accel.add(prim1);
  accel.add(prim2);

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
      if(accel.intersect(ray, res)) {
        img.setPixel(i, j, (res.hitNormal + 1)/2);
      }
      else {
        img.setPixel(i, j, Vec3(0, 0, 0));
      }
    }
  }
  img.ppm_output("output.ppm");
}
