#ifndef PT_EXPLICIT_H
#define PT_EXPLICIT_H
#include "../integrator.h"
#include "../timer.h"
class PtExplicit : public Integrator {
  public:
    int N;

    PtExplicit(const std::shared_ptr<Camera>& _camera, const std::shared_ptr<Sampler>& _sampler, int _N) : Integrator(_camera, _sampler), N(_N) {};

    RGB sampleLight(const Ray& ray, Scene& scene, Hit& res, const Vec3& wo_local, const Vec3& n, const Vec3& s, const Vec3& t, const std::shared_ptr<Light>& light) const {
      auto hitMaterial = res.hitPrimitive->material;

      //Light Sampling
      Vec3 col_light;
      float light_pdf = 0;
      Vec3 wi_light;
      Vec3 samplePos;
      RGB le = light->sample(res, *this->sampler, wi_light, samplePos, light_pdf);
      if(isZero(le) || light_pdf == 0) return RGB(0);
      Vec3 wi_light_local = worldToLocal(wi_light, n, s, t);

      //BRDF
      RGB k = hitMaterial->f(res, wo_local, wi_light_local) * std::max(cosTheta(wi_light_local), 0.0f);
      float brdf_pdf = hitMaterial->Pdf(wo_local, wi_light_local);
      if(isZero(k)) return RGB(0);

      //MIS Weight
      float weight = std::pow(light_pdf, 2.0f)/(std::pow(light_pdf, 2.0f) + std::pow(brdf_pdf, 2.0f));
      if(std::isinf(weight) || std::isnan(weight)) return RGB(0);

      //Visibility Test
      Ray shadowRay(res.hitPos, wi_light);
      Hit shadow_res;
      if(light->type == LIGHT_TYPE::AREA) {
        if(scene.intersect(shadowRay, shadow_res)) {
          if(shadow_res.hitPrimitive->light == light && (samplePos - shadow_res.hitPos).length2() < 1e-6) {
            col_light += weight * k * le/light_pdf;
          }
        }
      }
      else if(light->type == LIGHT_TYPE::SKY) {
        if(!scene.intersect(shadowRay, shadow_res)) {
          col_light += weight * k * le/light_pdf;
        }
      }
      //Handle Delta Light Case
      else if(light->type == LIGHT_TYPE::POINT) {
        scene.intersect(shadowRay, shadow_res);
        if(shadow_res.t >= (samplePos - shadowRay.origin).length()) {
          col_light += k * le/light_pdf;
        }
      }
      return col_light;
    };


    RGB sampleBRDF(const Ray& ray, Scene& scene, Hit& res, const Vec3& wo_local, const Vec3& n, const Vec3& s, const Vec3& t, const std::shared_ptr<Light>& light) const {
      Vec3 col_brdf;
      auto hitMaterial = res.hitPrimitive->material;

      //BRDF Sampling
      Vec3 wi_local;
      float brdf_pdf;
      RGB brdf = hitMaterial->sample(res, wo_local, *this->sampler, wi_local, brdf_pdf);
      if(isZero(brdf) || brdf_pdf == 0) return RGB(0);
      float cos = absCosTheta(wi_local);
      if(cos == 0) return RGB(0);
      Vec3 wi = localToWorld(wi_local, n, s, t);
      RGB k = brdf * cos/brdf_pdf;

      //Visibility Test
      Ray shadowRay(res.hitPos, wi);
      Hit shadow_res;
      if(light->type != LIGHT_TYPE::SKY) {
        if(scene.intersect(shadowRay, shadow_res)) {
          if(shadow_res.hitPrimitive->light != nullptr) {
            //Light
            RGB le = light->Le(shadow_res, shadowRay);
            float light_pdf = light->Pdf(res, wi, shadow_res);

            //MIS Weight
            float weight = std::pow(brdf_pdf, 2.0f)/(std::pow(brdf_pdf, 2.0f) + std::pow(light_pdf, 2.0f));
            if(std::isinf(weight) || std::isnan(weight)) return RGB(0);

            if(hitMaterial->type != MATERIAL_TYPE::SPECULAR) {
              col_brdf += weight * k * le;
            }
            //Handle Specular Case
            else {
              col_brdf += k * le;
            }
          }
        }
      }
      else {
        if(!scene.intersect(shadowRay , shadow_res)) {
          //Light
          RGB le = light->Le(shadow_res, shadowRay);
          float light_pdf = light->Pdf(res, wi, shadow_res);

          //MIS Weight
          float weight = std::pow(brdf_pdf, 2.0f)/(std::pow(brdf_pdf, 2.0f) + std::pow(light_pdf, 2.0f));
          if(std::isinf(weight) || std::isnan(weight)) return RGB(0);

          if(hitMaterial->type != MATERIAL_TYPE::SPECULAR) {
            col_brdf += weight * k * le;
          }
          //Handle Specular Case
          else {
            col_brdf += k * le;
          }
        }
      }
      return col_brdf;
    };


    RGB Li(const Ray& _ray, Scene& scene) const {
      float russian_roulette = 1.0f;
      Vec3 col;
      Vec3 col2(1);
      Ray ray = _ray;

      for(int depth = 0; ; depth++) {
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

          //Local Coordinate
          Vec3 wo = -ray.direction;
          Vec3 n = res.hitNormal;
          Vec3 s, t;
          orthonormalBasis(n, s, t);
          Vec3 wo_local = worldToLocal(wo, n, s, t);

          //Direct Illumination
          //Light Sampling
          unsigned int light_index = (int)(scene.lights.size() * (*this->sampler).getNext());
          if(light_index == scene.lights.size()) light_index--;
          const auto light = scene.lights[light_index];

          //Calc Direct Illumination
          Vec3 direct_col = scene.lights.size() * (sampleLight(ray, scene, res, wo_local, n, s, t, light) + sampleBRDF(ray, scene, res, wo_local, n, s, t, light));

          //if Direct Illumination is inf or nan
          if(isNan(direct_col) || isInf(direct_col)) {
            std::cerr << "NaN or Inf detected at Direct Illumination" << std::endl;
            break;
          }

          //Sample BRDF for next ray direction
          Vec3 wi_local;
          float brdf_pdf;
          RGB brdf = hitMaterial->sample(res, wo_local, *this->sampler, wi_local, brdf_pdf);
          float cos = absCosTheta(wi_local);
          if(isZero(brdf) || brdf_pdf == 0 || cos == 0) break;
          RGB k = brdf * cos / brdf_pdf;
          if(isNan(k) || isInf(k)) {
            std::cerr << "NaN or Inf detected at BRDF Sampling" << std::endl;
            std::cerr << "BRDF PDF: " << brdf_pdf << std::endl;
            std::cerr << "cos: " << cos << std::endl;
            break;
          }
          Vec3 wi = localToWorld(wi_local, n, s, t);

          //next ray
          ray = Ray(res.hitPos, wi);
          col += direct_col * col2;
          col2 *= k;

          //russian roulette
          if(depth > 3) {
            russian_roulette = std::max(col2.length()*0.577f, 0.05f);
            if((*this->sampler).getNext() > russian_roulette) {
              break;
            }
            col2 /= russian_roulette;
          }
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
