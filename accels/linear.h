#ifndef LINEAR_H
#define LINEAR_H
#include "../accel.h"
class Linear : public Accel {
  public:
    Linear() : Accel() {};

    bool intersect(const Ray& ray, Hit& res) const {
      bool hit = false;
      Hit res_temp;
      for(auto& p : this->primitives) {
        if(p->intersect(ray, res_temp)) {
          if(res_temp.t < res.t) {
            hit = true;
            res = res_temp;
          }
        }
      }
      return hit;
    };
};
#endif
