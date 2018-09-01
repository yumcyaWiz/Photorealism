#ifndef SKY_H
#define SKY_H
#include "../vec3.h"
#include "../ray.h"
#include "../light.h"
class Sky : public Light {
  public:
    Sky() : Light(LIGHT_TYPE::SKY) {};
};
#endif
