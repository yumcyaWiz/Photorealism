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
#include "lights/pointLight.h"
#include "lights/areaLight.h"
#include "scene.h"
#include "integrators/pt.h"
#include "integrators/pt_explicit.h"



int main() {
  int N = 100;
  auto film = std::make_shared<Film>(512, 512);
  auto camera = std::make_shared<ThinLensCamera>(Vec3(0, 1, 0), normalize(Vec3(0, 0, 5) - Vec3(0, 1, 0)), film, Vec3(0, 0, 5), 1, 2.8);

  auto floor = std::make_shared<Sphere>(Vec3(0, -10001, 0), 10000);
  auto sphere1 = std::make_shared<Sphere>(Vec3(-2, 0, 3), 1);
  auto sphere2 = std::make_shared<Sphere>(Vec3(0, 0, 5), 1);
  auto sphere3 = std::make_shared<Sphere>(Vec3(2, 0, 7), 1);

  auto mat = std::make_shared<Lambert>(RGB(0.9, 0.9, 0.9));
  auto blue = std::make_shared<Lambert>(RGB(0.2, 0.2, 0.8));
  auto mat2 = std::make_shared<Glass>(1.5, RGB(1));

  std::vector<std::shared_ptr<Light>> lights;
  auto light1 = std::make_shared<PointLight>(RGB(5), Vec3(0, 3, 5));
  lights.push_back(light1);

  std::vector<std::shared_ptr<Primitive>> prims;
  auto prim1 = std::make_shared<Primitive>(floor, blue);
  auto prim2 = std::make_shared<Primitive>(sphere1, mat);
  auto prim3 = std::make_shared<Primitive>(sphere2, mat);
  auto prim4 = std::make_shared<Primitive>(sphere3, mat);

  prims.push_back(prim1);
  prims.push_back(prim2);
  prims.push_back(prim3);
  prims.push_back(prim4);

  //auto sky = std::make_shared<IBL>("PaperMill_E_3k.hdr", 0.1, M_PI, 0);
  auto sky = std::make_shared<UniformSky>(RGB(0));
  Scene scene(prims, lights, sky);

  auto sampler = std::make_shared<Mt>();
  PtExplicit integrator(camera, sampler, N);

  integrator.render(scene);
  return 0;
}
