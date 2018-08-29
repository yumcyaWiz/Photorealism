#ifndef PT_EXPLICIT_H
#define PT_EXPLICIT_H
#include "../integrator.h"
class PtExplicit : public Integrator {
  public:
    int N;

    PtExplicit(const std::shared_ptr<Camera>& _camera, const std::shared_ptr<Sampler>& _sampler, int _N) : Integrator(_camera, _sampler), N(_N) {};

    RGB Li(const Ray& _ray, Scene& scene) const {
      float russian_roulette = 1.0f;
      Vec3 col;
      Vec3 col2(1);
      Ray ray = _ray;
      bool nee = false;

      for(int depth = 0; ; depth++) {
        if(depth > 10) {
          russian_roulette *= 0.95f;
        }
        if((*this->sampler).getNext() > russian_roulette) {
          break;
        }

        Hit res;
        if(scene.intersect(ray, res)) {
          if(res.hitPrimitive->light != nullptr) {
            if(!nee) {
              return res.hitPrimitive->light->Le(res);
            }
            else {
              break;
            }
          }

          auto hitMaterial = res.hitPrimitive->material;
          Vec3 wo = -ray.direction;
          Vec3 n = res.hitNormal;
          Vec3 s, t;
          orthonormalBasis(n, s, t);
          Vec3 wo_local = worldToLocal(wo, n, s, t);

          Vec3 direct_col;
          if(hitMaterial->type == MATERIAL_TYPE::DIFFUSE || hitMaterial->type == MATERIAL_TYPE::GLOSSY) {
            nee = true;
            for(const auto& light : scene.lights) {
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
                    direct_col += hitMaterial->f(wo_local, wi_light_local) * le/light_pdf * std::max(cosTheta(wi_light_local), 0.0f);
                  }
                }
              }
              else if(light->type == LIGHT_TYPE::POINT) {
                scene.intersect(shadowRay, shadow_res);
                if(shadow_res.t >= (samplePos - shadowRay.origin).length()) {
                  direct_col += hitMaterial->f(wo_local, wi_light_local) * le/light_pdf * std::max(cosTheta(wi_light_local), 0.0f);
                }
              }
              else {
              }
            }
          }
          if(isNan(direct_col) || isInf(direct_col)) {
            std::cerr << "NaN or Inf detected at Light Sampling" << std::endl;
            break;
          }

          Vec3 wi_local;
          float brdf_pdf;
          RGB brdf = hitMaterial->sample(wo_local, *this->sampler, wi_local, brdf_pdf);
          Vec3 wi = localToWorld(wi_local, n, s, t);
          float cos = std::max(cosTheta(wi_local), 0.0f);
          RGB k = brdf * cos / brdf_pdf;
          if(isNan(k) || isInf(k)) {
            std::cerr << "NaN or Inf detected at BRDF Sampling" << std::endl;
            break;
          }

          ray = Ray(res.hitPos, wi);
          col += direct_col * col2 / russian_roulette;
          col2 *= k / russian_roulette;
        }
        else {
          break;
        }
      }
      return col;
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
