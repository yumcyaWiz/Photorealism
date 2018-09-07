#ifndef AABB_H
#define AABB_H
#include <limits>
#include "vec3.h"
class AABB {
  public:
    Vec3 pMin;
    Vec3 pMax;

    AABB() {
      constexpr float INF_MIN = std::numeric_limits<float>::lowest();
      constexpr float INF_MAX = std::numeric_limits<float>::max();
      pMin = Vec3(INF_MAX);
      pMax = Vec3(INF_MIN);
    };
    AABB(const Vec3& p1, const Vec3& p2) {
      pMin = min(p1, p2);
      pMax = max(p1, p2);
    };

    float surfaceArea() const {
      float dx = pMax.x - pMin.x;
      float dy = pMax.y - pMin.y;
      float dz = pMax.z - pMin.z;
      return 2*(dx*dy + dy*dz + dz*dx);
    };

    bool intersect(const Ray& ray) const {
      float t0 = ray.tmin;
      float t1 = ray.tmax;
      for(int i = 0; i < 3; i++) {
        float invDir = 1.0f/ray.direction[i];
        float tNear = (pMin[i] - ray.origin[i])*invDir;
        float tFar = (pMax[i] - ray.origin[i])*invDir;
        if(tNear > tFar) std::swap(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if(t0 > t1) return false;
      }
      return true;
    };
};


inline std::ostream& operator<<(std::ostream& stream, const AABB& aabb) {
  stream << "pMin:" << aabb.pMin << std::endl;
  stream << "pMax:" << aabb.pMax << std::endl;
  return stream;
}


inline AABB mergeAABB(const AABB& b1, const AABB& b2) {
  return AABB(min(b1.pMin, b2.pMin), max(b1.pMax, b2.pMax));
}
inline AABB mergeAABB(const AABB& b, const Vec3& p) {
  return AABB(min(b.pMin, p), max(b.pMax, p));
}


inline int maximumExtent(const AABB& b) {
  float dx = b.pMax.x - b.pMin.x;
  float dy = b.pMax.y - b.pMin.y;
  float dz = b.pMax.z - b.pMin.z;
  float max = std::max(dx, std::max(dy, dz));
  if(max == dx)
    return 0;
  else if(max == dy)
    return 1;
  else 
    return 2;
}
#endif
