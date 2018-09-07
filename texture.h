#ifndef TEXTURE_H
#define TEXTURE_H
#include "vec3.h"
#include "hit.h"
class Texture {
  public:
    virtual getColor(const Hit& res) const = 0;
};
#endif
