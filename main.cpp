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
  int N = 10;
  auto film = std::make_shared<Film>(512, 512);
  auto camera = std::make_shared<ThinLensCamera>(Vec3(0, 1, -5), Vec3(0, 0, 1), film, Vec3(0, 0.5, 2.5), 1.3, 1.8);

  auto floor = std::make_shared<Sphere>(Vec3(0, -10001, 0), 10000);
  auto right_wall = std::make_shared<Sphere>(Vec3(10004, 0, 0), 10000);
  auto left_wall = std::make_shared<Sphere>(Vec3(-10004, 0, 0), 10000);
  auto back_wall = std::make_shared<Sphere>(Vec3(0, 0, 10005), 10000);
  auto top_wall = std::make_shared<Sphere>(Vec3(0, 10005, 0), 10000);
  auto left_ball = std::make_shared<Sphere>(Vec3(-1.5, 0, 3), 1);
  auto right_ball = std::make_shared<Sphere>(Vec3(1.5, 0, 2), 1);
  auto light_ball = std::make_shared<Sphere>(Vec3(0, 2, 3), 0.1);

  auto white = std::make_shared<Lambert>(RGB(0.9, 0.9, 0.9));
  auto blue = std::make_shared<Lambert>(RGB(0.2, 0.2, 0.8));
  auto green = std::make_shared<Lambert>(RGB(0.2, 0.8, 0.2));
  auto red = std::make_shared<Lambert>(RGB(0.8, 0.2, 0.2));

  std::vector<std::shared_ptr<Light>> lights;
  auto light1 = std::make_shared<AreaLight>(RGB(30), light_ball);
  auto light2 = std::make_shared<PointLight>(RGB(20), Vec3(0, 3, 0));
  lights.push_back(light1);
  //lights.push_back(light2);

  std::vector<std::shared_ptr<Primitive>> prims;
  auto prim1 = std::make_shared<Primitive>(floor, white);
  auto prim2 = std::make_shared<Primitive>(right_wall, red);
  auto prim3 = std::make_shared<Primitive>(left_wall, green);
  auto prim4 = std::make_shared<Primitive>(back_wall, white);
  auto prim5 = std::make_shared<Primitive>(top_wall, white);
  auto prim6 = std::make_shared<Primitive>(left_ball, white);
  auto prim7 = std::make_shared<Primitive>(right_ball, white);
  auto prim8 = std::make_shared<Primitive>(light_ball, white, light1);

  prims.push_back(prim1);
  prims.push_back(prim2);
  prims.push_back(prim3);
  prims.push_back(prim4);
  prims.push_back(prim5);
  prims.push_back(prim6);
  prims.push_back(prim7);
  prims.push_back(prim8);

  //auto sky = std::make_shared<IBL>("PaperMill_E_3k.hdr", 0.1, M_PI, 0);
  auto sky = std::make_shared<UniformSky>(RGB(0));
  Scene scene(prims, lights, sky);

  auto sampler = std::make_shared<Mt>();
  PtExplicit integrator(camera, sampler, N);

  integrator.render(scene);
  return 0;
}
