#ifndef CAMERA_H
#define CAMERA_H
#include <memory>
#include "vec2.h"
#include "vec3.h"
#include "sampler.h"
#include "film.h"
class Camera {
  public:
    Vec3 camPos;
    Vec3 camForward;
    Vec3 camRight;
    Vec3 camUp;
    std::shared_ptr<Film> film;

    Camera(const Vec3& _camPos, const Vec3& _camForward, const std::shared_ptr<Film>& _film) : camPos(_camPos), camForward(_camForward), film(_film) {
      camRight = normalize(cross(camForward, Vec3(0, 1, 0)));
      camUp = normalize(cross(camRight, camForward));
    };

    virtual bool getRay(float u, float v, Sampler& sampler, Ray& ray, float& weight) const = 0;
    virtual float We(const Ray& ray, Vec2& pFilm) const {
      return 0;
    };
    virtual float We_Pdf(const Ray& ray, float& pdf_pos, float& pdf_dir) const {
      return 0;
    };
};
#endif
