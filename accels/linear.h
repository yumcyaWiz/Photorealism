#ifndef LINEAR_H
#define LINEAR_H
#include "../accel.h"
template <typename T>
class Linear : public Accel<T> {
  public:
    Linear(const std::vector<std::shared_ptr<T>>& prims) : Accel<T>(prims) {};

    bool intersect(const Ray& ray, Hit& res) const {
      bool hit = false;
      Hit res_temp;
      for(auto& p : this->prims) {
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
