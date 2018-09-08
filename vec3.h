#ifndef VEC3_H
#define VEC3_H
#include <cmath>
#include <iostream>
class Vec3 {
  public:
    float x;
    float y;
    float z;

    Vec3() { x = y = z = 0; };
    Vec3(float _x) : x(_x), y(_x), z(_x) {};
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};

    float operator[](int i) const {
      if(i == 0) return x;
      else if(i == 1) return y;
      else return z;
    };
    Vec3 operator-() const {
      return Vec3(-x, -y, -z);
    };
    Vec3& operator+=(const Vec3& v) {
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
    };
    Vec3& operator*=(const Vec3& v) {
      x *= v.x;
      y *= v.y;
      z *= v.z;
      return *this;
    };

    float length() const {
      return std::sqrt(x*x + y*y + z*z);
    };
    float length2() const {
      return x*x + y*y + z*z;
    };
};
typedef Vec3 RGB;


inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
inline Vec3 operator+(const Vec3& v1, float k) {
  return Vec3(v1.x + k, v1.y + k, v1.z + k);
}
inline Vec3 operator+(float k, const Vec3& v2) {
  return Vec3(k + v2.x, k + v2.y, k + v2.z);
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
inline Vec3 operator-(const Vec3& v1, float k) {
  return Vec3(v1.x - k, v1.x - k, v1.z - k);
}
inline Vec3 operator-(float k, const Vec3& v2) {
  return Vec3(k - v2.x, k - v2.y, k - v2.z);
}

inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
inline Vec3 operator*(const Vec3& v1, float k) {
  return Vec3(v1.x * k, v1.y * k, v1.z * k);
}
inline Vec3 operator*(float k, const Vec3& v2) {
  return Vec3(k * v2.x, k * v2.y, k * v2.z);
}

inline Vec3 operator/(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}
inline Vec3 operator/(const Vec3& v1, float k) {
  return Vec3(v1.x / k, v1.y / k, v1.z / k);
}
inline Vec3 operator/(float k, const Vec3& v2) {
  return Vec3(k / v2.x, k / v2.y, k / v2.z);
}


inline Vec3 normalize(const Vec3& v) {
  return v/v.length();
}


inline float dot(const Vec3& v1, const Vec3& v2) {
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}


inline std::ostream& operator<<(std::ostream& stream, const Vec3& v) {
  stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return stream;
}


inline Vec3 pow(const Vec3& v, float p) {
  return Vec3(std::pow(v.x, p), std::pow(v.y, p), std::pow(v.z, p));
}


inline void orthonormalBasis(const Vec3& n, Vec3& vx, Vec3& vz) {
  if(std::abs(n.x) > 0.9) vx = Vec3(0, 1, 0);
  else vx = Vec3(1, 0, 0);

  vx = normalize(vx - n*dot(vx, n));
  vz = cross(n, vx);
}


inline Vec3 worldToLocal(const Vec3& w, const Vec3& n, const Vec3& s, const Vec3& t) {
    return Vec3(dot(s, w), dot(n, w), dot(t, w));
}
inline Vec3 localToWorld(const Vec3& w, const Vec3& n, const Vec3& s, const Vec3& t) {
    return Vec3(s.x*w.x + n.x*w.y + t.x*w.z, s.y*w.x + n.y*w.y + t.y*w.z, s.z*w.x + n.z*w.y + t.z*w.z);
}


inline bool isNan(const Vec3& v) {
  return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}
inline bool isInf(const Vec3& v) {
  return std::isinf(v.x) || std::isinf(v.y) || std::isinf(v.z);
}
inline bool isZero(const Vec3& v) {
  return v.x == 0 && v.y == 0 && v.z == 0;
}


inline Vec3 min(const Vec3& v1, const Vec3& v2) {
  return Vec3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}
inline Vec3 max(const Vec3& v1, const Vec3& v2) {
  return Vec3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}
#endif
