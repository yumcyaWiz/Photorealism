#ifndef BDPT_H
#define BDPT_H
#include "../integrator.h"
class BDPT : public Integrator {
  public:
    int N;

    BDPT(const std::shared_ptr<Camera>& _camera, std::shared_ptr<Sampler>& _sampler, int _N) : Integrator(_camera, _sampler), N(_N) {};

    void lightTrace(Scene& scene, const std::shared_ptr<Light>& light) const {
      float russian_rolette = 1.0f;
      RGB L;
      Ray ray;
      Vec3 normal;
      float pdf_pos, pdf_dir;
      RGB T = light->sample_Le(*this->sampler, ray, normal, pdf_pos, pdf_dir);
      T /= pdf_pos*pdf_dir;

      for(int depth = 0; depth < 10; depth++) {
        if(isZero(T)) break;

        Hit res;
        if(scene.intersect(ray, res)) {
          auto hitMaterial = res.hitPrimitive->material;

          Vec3 wo = -ray.direction;
          Vec3 n = res.hitNormal;
          T *= std::max(dot(wo, n), 0.0f);
          Vec3 s, t;
          orthonormalBasis(n, s, t);
          Vec3 wo_local = worldToLocal(wo, n, s, t);

          Vec3 wi_camera;
          float camera_pdf;
          Vec2 pFilm;
          float We = camera->sample_Wi(res, *sampler, wi_camera, camera_pdf, pFilm);
          Hit res_camera;
          bool camera_hit = scene.intersect(Ray(res.hitPos, wi_camera), res_camera);
          if(!camera_hit || (res_camera.hitPos - res.hitPos).length2() > (camera->camPos - res.hitPos).length2()) {
            RGB brdf = hitMaterial->f(res, wi_camera, wo);
            camera->film->addSample(pFilm, 20*brdf*We*T/camera_pdf);
          }

          Vec3 wi_local;
          float brdf_pdf;
          RGB brdf = hitMaterial->sample(res, wo_local, *this->sampler, wi_local, brdf_pdf);
          float cos = absCosTheta(wi_local);
          if(isZero(brdf) || brdf_pdf == 0 || cos == 0) break;
          RGB k = brdf * cos/brdf_pdf;
          if(isNan(k) || isInf(k)) break;
          Vec3 wi = localToWorld(wi_local, n, s, t);

          ray = Ray(res.hitPos, wi);
          T *= k;
        }
        else {
          break;
        }
      }
    };

    void render(Scene& scene) const {
      const int width = camera->film->width;
      const int height = camera->film->height;

#pragma omp parallel for schedule(dynamic, 1)
      for(int k = 0; k < width*height*N; k++) {
        unsigned int light_index = (int)(scene.lights.size() * sampler->getNext());
        if(light_index == scene.lights.size()) light_index--;
        const auto light = scene.lights[light_index];
        lightTrace(scene, light);

        if(omp_get_thread_num() == 0) {
          int index = k;
          std::cout << progressbar(index, width*height*N) << " " << percentage(index, width*height*N) << "\r" << std::flush;
        }
      }

      camera->film->ppm_output("output.ppm");
    };
};
#endif
