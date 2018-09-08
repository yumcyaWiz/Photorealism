#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include <memory>
#include "ray.h"
#include "hit.h"
#include "lights/sky.h"
#include "light.h"
#include "accel.h"
class Scene {
  public:
    std::shared_ptr<Accel<Primitive>> accel;
    std::vector<std::shared_ptr<Light>> lights;
    std::shared_ptr<Sky> sky;

    Scene(std::shared_ptr<Accel<Primitive>> _accel, const std::vector<std::shared_ptr<Light>>& _lights, const std::shared_ptr<Sky>& _sky) : accel(_accel), lights(_lights), sky(_sky) {
      lights.push_back(sky);
    };

    bool intersect(const Ray& ray, Hit& res) const {
      return accel->intersect(ray, res);
    };
};
#endif
