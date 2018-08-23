#include <iostream>
#include <memory>
#include <omp.h>
#include "vec3.h"
#include "ray.h"
#include "image.h"
#include "hit.h"
#include "shapes/sphere.h"
#include "cameras/pinholeCamera.h"
#include "materials/lambert.h"
#include "primitive.h"
#include "accels/linear.h"
#include "samplers/mt.h"


RGB Li(const Ray& ray, const Accel& accel, Sampler& sampler, int depth = 0) {
  if(depth > 10) return RGB(0, 0, 0);

  Hit res;
  if(accel.intersect(ray, res)) {
    auto hitMaterial = res.hitPrimitive->material;

    Vec3 n = res.hitNormal;
    Vec3 s, t;
    orthonormalBasis(n, s, t);

    Vec3 wo_local = worldToLocal(-ray.direction, n, s, t);
    Vec3 wi_local;
    double pdf;
    RGB brdf = hitMaterial->sample(wo_local, sampler, wi_local, pdf);
    double cos = absCosTheta(wi_local);
    Vec3 wi = localToWorld(wi_local, n, s, t);

    Ray nextRay(res.hitPos + 0.001*res.hitNormal, wi);

    return 1/pdf * brdf * cos * Li(nextRay, accel, sampler, depth + 1);
  }
  else {
    return RGB(1, 1, 1);
  }
}


int main() {
  Image img(512, 512);
  PinholeCamera cam(Vec3(), Vec3(0, 0, 1));
  Linear accel;

  auto sphere1 = std::make_shared<Sphere>(Vec3(0, 0, 3), 1);
  auto sphere2 = std::make_shared<Sphere>(Vec3(0, -10001, 0), 10000);

  auto mat = std::make_shared<Lambert>(RGB(0.9, 0.9, 0.9));
  auto mat2 = std::make_shared<Lambert>(RGB(0.2, 0.2, 0.8));

  auto prim1 = std::make_shared<Primitive>(sphere1, mat2);
  auto prim2 = std::make_shared<Primitive>(sphere2, mat);

  accel.add(prim1);
  accel.add(prim2);

  Mt sampler;

#pragma omp parallel for schedule(dynamic, 1)
  for(int k = 0; k < 100; k++) {
    for(int i = 0; i < img.width; i++) {
      for(int j = 0; j < img.height; j++) {
        double u = (2.0*i - img.width + sampler.getNext())/img.width;
        double v = (2.0*j - img.height + sampler.getNext())/img.width;

        Ray ray;
        if(!cam.getRay(u, v, ray)) {
          img.setPixel(i, j, Vec3(0, 0, 0));
        }
        else {
          RGB li = Li(ray, accel, sampler);
          img.setPixel(i, j, img.getPixel(i, j) + li);
        }
      }
    }
  }
  img.divide(100);
  img.gamma_correction();
  img.ppm_output("output.ppm");
}
