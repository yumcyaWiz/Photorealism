#ifndef PT_EXPLICIT_H
#define PT_EXPLICIT_H
#include "../integrator.h"
#include "../timer.h"
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

        if(isZero(col2)) break;

        Hit res;
        if(scene.intersect(ray, res)) {
          if(res.hitPrimitive->light != nullptr) {
            if(depth == 0) {
              return res.hitPrimitive->light->Le(res, ray);
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

          //Direct Illumination
          //Light Sampling
          Vec3 direct_col_light;
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
                  direct_col_light += hitMaterial->f(res, wo_local, wi_light_local) * le/light_pdf * std::max(cosTheta(wi_light_local), 0.0f) / light_selection_pdf;
                }
              }
            }
            else if(light->type == LIGHT_TYPE::POINT) {
              scene.intersect(shadowRay, shadow_res);
              if(shadow_res.t >= (samplePos - shadowRay.origin).length()) {
                direct_col_light += hitMaterial->f(res, wo_local, wi_light_local) * le/light_pdf * std::max(cosTheta(wi_light_local), 0.0f) / light_selection_pdf;
              }
            }
            else if(light->type == LIGHT_TYPE::SKY) {
              if(!scene.intersect(shadowRay, shadow_res)) {
                direct_col_light += hitMaterial->f(res, wo_local, wi_light_local) * le/light_pdf * std::max(cosTheta(wi_light_local), 0.0f) / light_selection_pdf;
              }
            }
          }

          //MIS Weight
          float l = std::pow(light_pdf, 2.0f);
          float b = std::pow(light_brdf_pdf, 2.0f);
          float w_light = l/(l + b);
          if(std::isinf(w_light) || std::isnan(w_light)) w_light = 0;


          //BRDF Sampling
          Vec3 direct_col_brdf;
          Vec3 wi_local;
          float brdf_pdf = 0;
          float brdf_light_pdf = 0;
          RGB brdf = hitMaterial->sample(res, wo_local, *this->sampler, wi_local, brdf_pdf);
          float cos = absCosTheta(wi_local);
          Vec3 wi = localToWorld(wi_local, n, s, t);
          RGB k = brdf * cos/brdf_pdf;

          if(brdf_pdf != 0) {
            Ray shadowRay = Ray(res.hitPos, wi);
            Hit shadow_res;

            if(scene.intersect(shadowRay, shadow_res)) {
              if(shadow_res.hitPrimitive->light != nullptr) {
                direct_col_brdf += k * shadow_res.hitPrimitive->light->Le(shadow_res, shadowRay);
                brdf_light_pdf = shadow_res.hitPrimitive->light->Pdf(res, wi, shadow_res);
              }
            }
            else {
              direct_col_brdf += k * scene.sky->Le(shadow_res, shadowRay);
              brdf_light_pdf = scene.sky->Pdf(res, wi, shadow_res);
            }
          }
          //handle Specular Case
          if(hitMaterial->type == MATERIAL_TYPE::SPECULAR) {
            brdf_light_pdf = 0;
          }

          //MIS Weight
          l = std::pow(brdf_light_pdf, 2.0f);
          b = std::pow(brdf_pdf, 2.0f);
          float w_brdf = b/(l + b);
          if(std::isinf(w_brdf) || std::isnan(w_brdf)) w_brdf = 0;


          //Calc Direct Illumination
          Vec3 direct_col = w_light*direct_col_light + w_brdf*direct_col_brdf;


          //if Direct Illumination is inf or nan
          if(isNan(direct_col) || isInf(direct_col)) {
            std::cerr << "NaN or Inf detected at Direct Illumination" << std::endl;
            break;
          }


          //next ray
          brdf_pdf = 0;
          brdf = hitMaterial->sample(res, wo_local, *this->sampler, wi_local, brdf_pdf);
          if(brdf_pdf == 0) break;
          wi = localToWorld(wi_local, n, s, t);
          cos = absCosTheta(wi_local);
          if(cos == 0) break;
          k = brdf * cos / brdf_pdf;

          if(isNan(k) || isInf(k)) {
            std::cerr << "NaN or Inf detected at BRDF Sampling" << std::endl;
            std::cerr << "BRDF: " << brdf << std::endl;
            std::cerr << "cos: " << cos << std::endl;
            std::cerr << "BRDF PDF: " << brdf_pdf << std::endl;
            std::cerr << "wi_local: " << wi_local << std::endl;
            std::cerr << "wi: " << wi << std::endl;
            break;
          }

          ray = Ray(res.hitPos, wi);
          col += direct_col * col2 / russian_roulette;
          col2 *= k / russian_roulette;
        }
        else {
          if(depth == 0) {
            return scene.sky->Le(res, ray);
          }
          else {
            break;
          }
        }
      }
      return col;
    };

    void render(Scene& scene) const {
      const int width = this->camera->film->width;
      const int height = this->camera->film->height;

      Timer timer;
      int ms = 0;
      for(int k = 0; k < N; k++) {
        timer.start();
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
              std::cout << progressbar(width*height*k + height*i + j, width*height*N) << " " << percentage(width*height*k + height*i + j, width*height*N) << " ETA: " << ms*(N - k)/1e3 << "s" << "\r" << std::flush;
            }
          }
        }
        ms = timer.stop();
      }
      this->camera->film->divide(N);
      this->camera->film->gamma_correction();
      this->camera->film->ppm_output("output.ppm");
    };
};
#endif
