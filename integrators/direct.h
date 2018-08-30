#ifndef DIRECT_H
#define DIRECT_H
#include "../integrator.h"
class Direct : public Integrator {
  public:
    int N;

    Direct(const std::shared_ptr<Camera>& _camera, const std::shared_ptr<Sampler>& _sampler, int _N) : Integrator(_camera, _sampler), N(_N) {};

    RGB Li(const Ray& ray, Scene& scene) const {
      Hit res;
      if(scene.intersect(ray, res)) {
        if(res.hitPrimitive->light != nullptr) {
          return res.hitPrimitive->light->Le(res);
        }

        auto hitMaterial = res.hitPrimitive->material;
        Vec3 wo = -ray.direction;
        Vec3 n = res.hitNormal;
        Vec3 s, t;
        orthonormalBasis(n, s, t);
        Vec3 wo_local = worldToLocal(wo, n, s, t);

        //Light Sampling
        Vec3 col_light;
        unsigned int light_index = (int)(scene.lights.size() * (*this->sampler).getNext());
        if(light_index == scene.lights.size()) light_index--;
        auto light = scene.lights[light_index];
        float light_selection_pdf = 1.0f/scene.lights.size();
        float light_pdf;
        Vec3 wi_light;
        Vec3 samplePos;
        RGB le = light->sample(res, *this->sampler, wi_light, samplePos, light_pdf);
        Vec3 wi_light_local = worldToLocal(wi_light, n, s, t);

        Ray shadowRay(res.hitPos, wi_light);
        Hit shadow_res;

        if(light->type == LIGHT_TYPE::AREA) {
          if(scene.intersect(shadowRay, shadow_res)) {
            if(shadow_res.hitPrimitive->light == light && (samplePos - shadow_res.hitPos).length2() < 1e-6) {
              col_light += hitMaterial->f(wo_local, wi_light_local) * le/light_pdf * std::max(cosTheta(wi_light_local), 0.0f)/light_selection_pdf;
            }
          }
        }
        else if(light->type == LIGHT_TYPE::POINT) {
          scene.intersect(shadowRay, shadow_res);
          if(shadow_res.t >= (samplePos - shadowRay.origin).length()) {
            col_light += hitMaterial->f(wo_local, wi_light_local) * le/light_pdf * std::max(cosTheta(wi_light_local), 0.0f)/light_selection_pdf;
          }
        }

        //BRDF Sampling
        Vec3 col_brdf;
        Vec3 wi_local;
        float brdf_pdf;
        RGB brdf = hitMaterial->sample(wo_local, *this->sampler, wi_local, brdf_pdf);
        float cos = absCosTheta(wi_local);
        Vec3 wi = localToWorld(wi_local, n, s, t);
        RGB k = brdf * cos/brdf_pdf;

        shadowRay = Ray(res.hitPos, wi);
        if(scene.intersect(shadowRay, shadow_res)) {
          if(shadow_res.hitPrimitive->light != nullptr) {
            col_brdf += k * shadow_res.hitPrimitive->light->Le(shadow_res);
          }
        }
        else {
          col_brdf += k * scene.sky->getColor(shadowRay);
        }

        //MIS
        float l = std::pow(light_pdf, 2.0f);
        float b = std::pow(brdf_pdf, 2.0f);
        float denom = l + b;
        //return RGB(1, 0, 0)*l/denom + RGB(0, 1, 0)*b/denom;
        return l/denom * col_light + b/denom * col_brdf;
      }
      else {
        return scene.sky->getColor(ray);
      }
    };

    void render(Scene& scene) const {
      const int width = this->camera->film->width;
      const int height = this->camera->film->height;

      for(int k = 0; k < N; k++) {
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

            if(omp_get_thread_num() == 0) {
              std::cout << progressbar(k, N) << " " << percentage(k, N) << "\r" << std::flush;
            }
          }
        }
      }
      this->camera->film->divide(N);
      this->camera->film->gamma_correction();
      this->camera->film->ppm_output("output.ppm");
    };
};
#endif
