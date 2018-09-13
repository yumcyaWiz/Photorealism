#ifndef PT_H
#define PT_H
#include "../vec3.h"
#include "../ray.h"
#include "../hit.h"
#include "../camera.h"
#include "../sampler.h"
#include "../scene.h"
#include "../material.h"
#include "../integrator.h"
#include "../timer.h"
class Pt : public Integrator {
  public:
    int N;

    Pt(const std::shared_ptr<Camera>& _camera, const std::shared_ptr<Sampler>& _sampler, int _N) : Integrator(_camera, _sampler), N(_N) {};

    RGB Li(const Ray& ray, Scene& scene) const {
      RGB col(1);
      Ray trace_ray = ray;
      float russian_roulette = 1.0;

      for(int depth = 0; ; depth++) {
        if(depth > 3) {
          russian_roulette = std::max(col.length()*0.577f, 0.05f);
        }
        if((*this->sampler).getNext() > russian_roulette) {
          col = RGB(0);
          break;
        }

        Hit res;
        if(scene.intersect(trace_ray, res)) {
          if(res.hitPrimitive->light != nullptr) {
            col *= res.hitPrimitive->light->Le(res, trace_ray);
            break;
          }

          auto hitMaterial = res.hitPrimitive->material;

          Vec3 n = res.hitNormal;
          Vec3 s, t;
          orthonormalBasis(n, s, t);

          Vec3 wo_local = worldToLocal(-trace_ray.direction, n, s, t);
          Vec3 wi_local;
          float pdf;
          RGB brdf = hitMaterial->sample(res, wo_local, *this->sampler, wi_local, pdf);
          float cos = absCosTheta(wi_local);
          Vec3 wi = localToWorld(wi_local, n, s, t);

          trace_ray = Ray(res.hitPos, wi);

          RGB k = brdf * cos / pdf;
          if(isInf(k) || isNan(k)) {
            std::cerr << "Inf or NaN detected" << std::endl;
            std::cerr << "BRDF PDF: " << pdf << std::endl;
            break;
          }

          col *= 1/russian_roulette * k;
        }
        else {
          col *= 1/russian_roulette * scene.sky->Le(res, trace_ray);
          break;
        }
      }

      return col;
    };

    void render(Scene& scene) const {
      const int width = this->camera->film->width;
      const int height = this->camera->film->height;

      const int N_sqrt = std::sqrt(N);

      Timer timer;
      int ms = 0;
      for(int i = 0; i < width; i++) {
        timer.start();
        for(int j = 0; j < height; j++) {
#pragma omp parallel for schedule(dynamic, 1)
          for(int k = 0; k < N; k++) {
            float sx = k%N_sqrt * 2.0/width + 2.0/width*sampler->getNext();
            float sy = k/N_sqrt * 2.0/height + 2.0/height*sampler->getNext();
            float u = (2.0*i - width + sx)/width;
            float v = (2.0*j - height + sy)/width;
            Vec2 uv(u, v);

            Ray ray;
            float weight;
            if(!this->camera->getRay(u, v, *(this->sampler), ray, weight)) {
              this->camera->film->addSample(uv, RGB(0, 0, 0));
            }
            else {
              RGB li = weight*this->Li(ray, scene);
              this->camera->film->addSample(uv, li);
            }

            if(omp_get_thread_num() == 0) {
              int index = k + N*j + N*height*i;
              std::cout << progressbar(index, width*height*N) << " " << percentage(index, width*height*N) << " ETA: " << ms*(width - i)/1e3 << "s" << "\r" << std::flush;
            }
          }
        }
        ms = timer.stop();
      }
      this->camera->film->ppm_output("output.ppm");
    };
};
#endif
