#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include <memory>
#include "ray.h"
#include "hit.h"
#include "primitive.h"
#include "lights/sky.h"
#include "light.h"
#include "accels/linear.h"
class Scene {
  public:
    std::vector<std::shared_ptr<Primitive>> prims;
    std::vector<std::shared_ptr<Light>> lights;
    std::shared_ptr<Sky> sky;
    std::shared_ptr<Accel> accel;

    Scene(const std::vector<std::shared_ptr<Primitive>>& _prims, const std::vector<std::shared_ptr<Light>>& _lights, const std::shared_ptr<Sky>& _sky) : prims(_prims), lights(_lights), sky(_sky) {
      accel = std::make_shared<Linear>(prims);
      lights.push_back(sky);
    };

    bool intersect(const Ray& ray, Hit& res) const {
      return accel->intersect(ray, res);
    };
};
#endif
