#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H
#include <cmath>
#include "../texture.h"
class Checkerboard : public Texture {
  public:
    std::shared_ptr<Texture> odd;
    std::shared_ptr<Texture> even;
    float alphaX;
    float alphaY;
    float alphaZ;

    Checkerboard(const std::shared_ptr<Texture>& _odd, const std::shared_ptr<Texture>& _even, float _alphaX, float _alphaY, float _alphaZ) : odd(_odd), even(_even), alphaX(_alphaX), alphaY(_alphaY), alphaZ(_alphaZ) {};

    RGB getColor(const Hit& res) const {
      float f = std::sin(res.hitPos.x*alphaX*2*M_PI) * std::sin(res.hitPos.y*alphaY*2*M_PI) * std::sin(res.hitPos.z*alphaZ*2*M_PI);
      if(f > 0)
        return odd->getColor(res);
      else
        return even->getColor(res);
    };
};
#endif
