#include <iostream>
#include <memory>
#include <omp.h>
#include "vec3.h"
#include "ray.h"
#include "hit.h"
#include "shapes/sphere.h"
#include "cameras/pinholeCamera.h"
#include "cameras/thinLensCamera.h"
#include "materials/lambert.h"
#include "materials/mirror.h"
#include "materials/glass.h"
#include "primitive.h"
#include "samplers/mt.h"
#include "skys/ibl.h"
#include "skys/uniformSky.h"
#include "scene.h"
#include "integrators/pt.h"



int main() {
  int N = 10;
  auto film = std::make_shared<Film>(512, 512);
  auto camera = std::make_shared<ThinLensCamera>(Vec3(0, 1, 0), normalize(Vec3(0, 0, 5) - Vec3(0, 1, 0)), film, Vec3(0, 0, 5), 1, 2.8);

  auto floor = std::make_shared<Sphere>(Vec3(0, -10001, 0), 10000);
  auto sphere1 = std::make_shared<Sphere>(Vec3(-2, 0, 3), 1);
  auto sphere2 = std::make_shared<Sphere>(Vec3(0, 0, 5), 1);
  auto sphere3 = std::make_shared<Sphere>(Vec3(2, 0, 7), 1);

  auto mat = std::make_shared<Lambert>(RGB(0.9, 0.9, 0.9));
  auto mat2 = std::make_shared<Glass>(1.5, RGB(1));
  auto mat3 = std::make_shared<Glass>(1.5, RGB(1));
  auto mat4 = std::make_shared<Glass>(1.5, RGB(1));

  std::vector<std::shared_ptr<Primitive>> prims;
  auto prim1 = std::make_shared<Primitive>(floor, mat);
  auto prim2 = std::make_shared<Primitive>(sphere1, mat2);
  auto prim3 = std::make_shared<Primitive>(sphere2, mat3);
  auto prim4 = std::make_shared<Primitive>(sphere3, mat4);

  prims.push_back(prim1);
  prims.push_back(prim2);
  prims.push_back(prim3);
  prims.push_back(prim4);

  auto sky = std::make_shared<IBL>("PaperMill_E_3k.hdr", 0.1, M_PI, 0);
  Scene scene(prims, sky);

  auto sampler = std::make_shared<Mt>();
  Pt integrator(camera, sampler, N);

  integrator.render(scene);
  return 0;
}
