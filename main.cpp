#include <iostream>
#include <memory>
#include <omp.h>
#include "vec3.h"
#include "ray.h"
#include "image.h"
#include "hit.h"
#include "shapes/sphere.h"
#include "cameras/pinholeCamera.h"
#include "cameras/thinLensCamera.h"
#include "materials/lambert.h"
#include "materials/mirror.h"
#include "materials/glass.h"
#include "primitive.h"
#include "accels/linear.h"
#include "samplers/mt.h"
#include "skys/ibl.h"
#include "skys/uniformSky.h"


RGB Li(const Ray& ray, const Accel& accel, Sampler& sampler, const Sky& sky, int depth = 0) {
  if(depth > 100) return RGB(0, 0, 0);

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

    Ray nextRay(res.hitPos, wi);

    return 1/pdf * brdf * cos * Li(nextRay, accel, sampler, sky, depth + 1);
  }
  else {
    return sky.getColor(ray);
  }
}


int main() {
  int N = 300;
  Image img(512, 512);
  //PinholeCamera cam(Vec3(0, 1, 0), normalize(Vec3(0, 0, 5) - Vec3(0, 1, 0)));
  ThinLensCamera cam(Vec3(0, 1, 0), normalize(Vec3(0, 0, 5) - Vec3(0, 1, 0)), Vec3(0, 0, 5), 1, 1.4);
  Linear accel;

  auto floor = std::make_shared<Sphere>(Vec3(0, -10001, 0), 10000);
  auto sphere1 = std::make_shared<Sphere>(Vec3(-2, 0, 3), 1);
  auto sphere2 = std::make_shared<Sphere>(Vec3(0, 0, 5), 1);
  auto sphere3 = std::make_shared<Sphere>(Vec3(2, 0, 7), 1);

  auto mat = std::make_shared<Lambert>(RGB(0.9, 0.9, 0.9));
  auto mat2 = std::make_shared<Glass>(1.5, RGB(0.2, 0.2, 0.8));
  auto mat3 = std::make_shared<Glass>(1.5, RGB(0.2, 0.8, 0.2));
  auto mat4 = std::make_shared<Glass>(1.5, RGB(0.8, 0.2, 0.2));

  auto prim1 = std::make_shared<Primitive>(floor, mat);
  auto prim2 = std::make_shared<Primitive>(sphere1, mat2);
  auto prim3 = std::make_shared<Primitive>(sphere2, mat3);
  auto prim4 = std::make_shared<Primitive>(sphere3, mat4);

  accel.add(prim1);
  accel.add(prim2);
  accel.add(prim3);
  accel.add(prim4);

  Mt sampler;
  IBL sky("PaperMill_E_3k.hdr", 0.1, M_PI, 0);
  //UniformSky sky(RGB(1));

  for(int k = 0; k < N; k++) {
    for(int i = 0; i < img.width; i++) {
#pragma omp parallel for schedule(dynamic, 1)
      for(int j = 0; j < img.height; j++) {
        double u = (2.0*i - img.width + sampler.getNext())/img.width;
        double v = (2.0*j - img.height + sampler.getNext())/img.width;

        Ray ray;
        if(!cam.getRay(u, v, sampler, ray)) {
          img.setPixel(i, j, Vec3(0, 0, 0));
        }
        else {
          RGB li = Li(ray, accel, sampler, sky);
          img.setPixel(i, j, img.getPixel(i, j) + li);
        }

        if(omp_get_thread_num() == 0) {
          std::cout << progressbar(k, N) << " " << percentage(k, N) << "\r" << std::flush;
        }
      }
    }
  }
  img.divide(N);
  img.gamma_correction();
  img.ppm_output("output.ppm");
}
