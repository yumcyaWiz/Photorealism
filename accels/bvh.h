#ifndef BVH_H
#define BVH_H
#include "../accel.h"

enum class BVH_PARTITION_TYPE {
  MEDIAN,
  EQSIZE,
  SAH
};

class BVH : public Accel {
  public:
};
#endif
