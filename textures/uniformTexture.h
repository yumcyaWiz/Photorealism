#ifndef UNIFORM_TEXTURE_H
#define UNIFORM_TEXTURE_H
#include "../texture.h"
class UniformTexture : public Texture {
  public:
    RGB col;

    UniformTexture(const RGB& _col) : col(_col) {};

    RGB getColor(const Hit& res) const {
      return col;
    };
};
#endif
