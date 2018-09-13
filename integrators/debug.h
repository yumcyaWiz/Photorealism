#ifndef DEBUG_H
#define DEBUG_H
#include "../integrator.h"
class Debug : public Integrator {
  public:

    Debug(const std::shared_ptr<Camera>& _camera, const std::shared_ptr<Sampler>& _sampler) : Integrator(_camera, _sampler) {};

    RGB Li(const Ray& ray, Scene& scene) const {
      Hit res;
      scene.intersect(ray, res);
      return ray.hitCount * RGB(0, 1, 0);
    };

    void render(Scene& scene) const {
      const int width = this->camera->film->width;
      const int height = this->camera->film->height;

      for(int i = 0; i < width; i++) {
#pragma omp parallel for schedule(dynamic, 1)
        for(int j = 0; j < height; j++) {
          float u = (2.0*i - width)/width;
          float v = (2.0*j - height)/width;

          Ray ray;
          float weight;
          if(!this->camera->getRay(u, v, *(this->sampler), ray, weight)) {
            this->camera->film->addSample(i, j, RGB(0, 0, 0));
          }
          else {
            RGB li = weight*this->Li(ray, scene);
            this->camera->film->addSample(i, j, li);
          }
        }
      }


      float max = 0;
      for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
          if(this->camera->film->getPixel(i, j).y > max) {
            max = this->camera->film->getPixel(i, j).y;
          }
        }
      }
      this->camera->film->ppm_output("output.ppm");
    };
};
#endif
