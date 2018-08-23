#ifndef MT_H
#define MT_H
#include <random>
#include "../sampler.h"
#include "../vec2.h"
class Mt : public Sampler {
  public:
    std::random_device rnd_dev;
    std::mt19937 mt;
    std::uniform_real_distribution<double> dist;

    Mt() {
      mt.seed(rnd_dev());
      dist = std::uniform_real_distribution<double>(0, 1);
    };

    double getNext() {
      return dist(mt);
    };
    Vec2 getNext2D() {
      return Vec2(this->getNext(), this->getNext());
    };
};
#endif
