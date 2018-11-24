#ifndef DEBUG_NORMAL_H
#define DEBUG_NORMAL_H
#include "../integrator.h"
#include "../textures/imageTexture.h"
class DebugNormal : public Integrator {
  public:
    DebugNormal(const std::shared_ptr<Camera>& _camera, const std::shared_ptr<Sampler>& _sampler) : Integrator(_camera, _sampler) {};

    RGB Li(const Ray& ray, Scene& scene) const {
      Hit res;
      if(scene.intersect(ray, res)) {
        return (res.hitNormal + 1)/2;
      }
      else {
        return RGB(0);
      }
    };

    void render(Scene& scene) const {
      const int width = this->camera->film->width;
      const int height = this->camera->film->height;

#pragma omp parallel for schedule(dynamic, 1)
      for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
          float u = (2.0*(i + 0.5f) - width)/height;
          float v = (2.0*(j + 0.5f) - height)/height;
          Vec2 uv(u, v);

          Ray ray;
          float weight = 1.0f;
          if(!this->camera->getRay(u, v, *(this->sampler), ray, weight)) {
            this->camera->film->addSample(uv, RGB(0, 0, 0));
          }
          else {
            RGB li = weight*this->Li(ray, scene);
            this->camera->film->addSample(uv, li);
          }
        }
      }
      this->camera->film->ppm_output("output.ppm");
    };
};
#endif
