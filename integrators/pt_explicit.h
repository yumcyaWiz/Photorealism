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

      for(int depth = 0; ; depth++) {
        if(depth > 10) {
          russian_roulette *= 0.95f;
        }
        if((*this->sampler).getNext() > russian_roulette) {
          break;
        }

        if(col2.x == 0 && col2.y == 0 && col2.z == 0) break;

        Hit res;
        if(scene.intersect(ray, res)) {
          if(res.hitPrimitive->light != nullptr) {
            if(depth == 0) {
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

          //Direct Illumination with One Sample MIS
          Vec3 direct_col;
          //Russian Roulette
          bool light_or_brdf = (*this->sampler).getNext() > 0.5;

          //Light Sampling
          if(light_or_brdf) {
            unsigned int light_index = (int)(scene.lights.size() * (*this->sampler).getNext());
            if(light_index == scene.lights.size()) light_index--;
            float light_selection_pdf = 1.0/scene.lights.size();
            const auto light = scene.lights[light_index];
            float light_pdf = 0;
            Vec3 wi_light;
            Vec3 samplePos;
            RGB le = light->sample(res, *this->sampler, wi_light, samplePos, light_pdf);
            Vec3 wi_light_local = worldToLocal(wi_light, n, s, t);

            //handle Specular Case
            if(hitMaterial->type == MATERIAL_TYPE::SPECULAR) {
              light_pdf = 0;
            }

            //BRDF PDF
            float light_brdf_pdf = hitMaterial->Pdf(wo, wi_light);

            Ray shadowRay(res.hitPos, wi_light);
            Hit shadow_res;

            if(light_pdf != 0) {
              if(light->type == LIGHT_TYPE::AREA) {
                if(scene.intersect(shadowRay, shadow_res)) { 
                  if(shadow_res.hitPrimitive->light == light && (samplePos - shadow_res.hitPos).length2() < 1e-6) {
                    direct_col += hitMaterial->f(wo_local, wi_light_local) * le/light_pdf * std::max(cosTheta(wi_light_local), 0.0f) / light_selection_pdf / 0.5;
                  }
                }
              }
              else if(light->type == LIGHT_TYPE::POINT) {
                scene.intersect(shadowRay, shadow_res);
                if(shadow_res.t >= (samplePos - shadowRay.origin).length()) {
                  direct_col += hitMaterial->f(wo_local, wi_light_local) * le/light_pdf * std::max(cosTheta(wi_light_local), 0.0f) / light_selection_pdf / 0.5;
                }
              }
            }

            //MIS Weight
            float l = std::pow(light_pdf, 2.0f);
            float b = std::pow(light_brdf_pdf, 2.0f);
            float w_light = l/(l + b);
            if(std::isinf(w_light) || std::isnan(w_light)) w_light = 0;
            direct_col *= w_light;
          }
          //BRDF Sampling
          else {
            Vec3 wi_local;
            float brdf_pdf = 0;
            float brdf_light_pdf = 0;
            RGB brdf = hitMaterial->sample(wo_local, *this->sampler, wi_local, brdf_pdf);
            float cos = std::max(cosTheta(wi_local), 0.0f);
            Vec3 wi = localToWorld(wi_local, n, s, t);
            RGB k = brdf * cos/brdf_pdf / 0.5;

            if(brdf_pdf != 0) {
              Ray shadowRay = Ray(res.hitPos, wi);
              Hit shadow_res;

              if(scene.intersect(shadowRay, shadow_res)) {
                if(shadow_res.hitPrimitive->light != nullptr) {
                  direct_col += k * shadow_res.hitPrimitive->light->Le(shadow_res);

                  //Light PDF
                  brdf_light_pdf = shadow_res.hitPrimitive->light->Pdf(res, wi, shadow_res);
                  //handle Specular Case
                  if(hitMaterial->type == MATERIAL_TYPE::SPECULAR) {
                    brdf_light_pdf = 0;
                  }
                }
              }
              else {
                direct_col += k * scene.sky->getColor(shadowRay);
              }
            }

            //MIS Weight
            float l = std::pow(brdf_light_pdf, 2.0f);
            float b = std::pow(brdf_pdf, 2.0f);
            float w_brdf = b/(l + b);
            if(std::isinf(w_brdf) || std::isnan(w_brdf)) w_brdf = 0;
            direct_col *= w_brdf;
          }


          //if Direct Illumination is inf or nan
          if(isNan(direct_col) || isInf(direct_col)) {
            std::cerr << "NaN or Inf detected at Direct Illumination" << std::endl;
            break;
          }


          //next ray
          Vec3 wi_local;
          float brdf_pdf = 0;
          RGB brdf = hitMaterial->sample(wo_local, *this->sampler, wi_local, brdf_pdf);
          if(brdf_pdf == 0) break;
          Vec3 wi = localToWorld(wi_local, n, s, t);
          float cos = std::max(cosTheta(wi_local), 0.0f);
          RGB k = brdf * cos / brdf_pdf;

          if(isNan(k) || isInf(k)) {
            std::cerr << "NaN or Inf detected at BRDF Sampling" << std::endl;
            std::cerr << "BRDF PDF: " << brdf_pdf << std::endl;
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
              std::cout << progressbar(width*height*k + height*i + j, width*height*N) << " " << percentage(width*height*k + height*i + j, width*height*N) << "\r" << std::flush;
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
