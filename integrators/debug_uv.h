#ifndef DEBUG_UV_H
#define DEBUG_UV_H
#include "../integrator.h"
#include "../textures/imageTexture.h"
class DebugUV : public Integrator {
  private:
    std::shared_ptr<Texture> uv_img;

  public:
    DebugUV(const std::shared_ptr<Camera>& _camera, const std::shared_ptr<Sampler>& _sampler) : Integrator(_camera, _sampler) {
      uv_img = std::make_shared<ImageTexture>("uv_test.png");
    };

    RGB Li(const Ray& ray, Scene& scene) const {
      Hit res;
      if(scene.intersect(ray, res)) {
        if(res.u < 0 || res.u > 1.0 || res.v < 0 || res.v > 1.0) {
          std::cout << res.u << ", " << res.v << std::endl;
        }
        return uv_img->getColor(res);
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
