#ifndef UNIFORMSKY_H
#define UNIFORMSKY_H
#include "../sky.h"
#include "../vec3.h"
#include "../ray.h"
class UniformSky : public Sky {
  public:
    RGB color;

    UniformSky(const RGB& _color) : color(_color) {};

    RGB getColor(const Ray& ray) const {
      return color;
    };
};
#endif
