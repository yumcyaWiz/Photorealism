#ifndef DEBUG_H
#define DEBUG_H
#include "../integrator.h"
class Debug : public Integrator {
  public:

    Debug(const std::shared_ptr<Camera>& _camera, const std::shared_ptr<Sampler>& _sampler) : Integrator(_camera, _sampler) {};

    RGB Li(const Ray& ray, Scene& scene) const {
      Hit res;
      scene.intersect(ray, res);
      return ray.hitCount/10.0 * RGB(0, 1, 0);
    };

    void render(Scene& scene) const {
      const int width = this->camera->film->width;
      const int height = this->camera->film->height;

      for(int i = 0; i < width; i++) {
#pragma omp parallel for schedule(dynamic, 1)
        for(int j = 0; j < height; j++) {
          float u = (2.0*i - width + sampler->getNext())/width;
          float v = (2.0*j - height + sampler->getNext())/width;

          Ray ray;
          float weight;
          if(!this->camera->getRay(u, v, *(this->sampler), ray, weight)) {
            this->camera->film->setPixel(i, j, Vec3(0, 0, 0));
          }
          else {
            RGB li = weight*this->Li(ray, scene);
            this->camera->film->setPixel(i, j, this->camera->film->getPixel(i, j) + li);
          }
        }
      }
      this->camera->film->gamma_correction();
      this->camera->film->ppm_output("output.ppm");
    };
};
#endif
