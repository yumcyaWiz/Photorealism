#ifndef INTEGRATOR_H
#define INTEGRATOR_H
#include "scene.h"
class Integrator {
  public:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Sampler> sampler;

    Integrator(const std::shared_ptr<Camera>& _camera, const std::shared_ptr<Sampler>& _sampler) : camera(_camera), sampler(_sampler) {};

    virtual void render(Scene& scene) const = 0;
};
#endif
