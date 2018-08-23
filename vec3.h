#ifndef VEC3_H
#define VEC3_H
#include <cmath>
#include <iostream>
class Vec3 {
  public:
    double x;
    double y;
    double z;

    Vec3() { x = y = z = 0; };
    Vec3(double _x) : x(_x), y(_x), z(_x) {};
    Vec3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};

    Vec3 operator-() const {
      return Vec3(-x, -y, -z);
    };

    double length() const {
      return std::sqrt(x*x + y*y + z*z);
    };
    double length2() const {
      return x*x + y*y + z*z;
    };
};
typedef Vec3 RGB;


inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
inline Vec3 operator+(const Vec3& v1, double k) {
  return Vec3(v1.x + k, v1.y + k, v1.z + k);
}
inline Vec3 operator+(double k, const Vec3& v2) {
  return Vec3(k + v2.x, k + v2.y, k + v2.z);
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
inline Vec3 operator-(const Vec3& v1, double k) {
  return Vec3(v1.x - k, v1.x - k, v1.z - k);
}
inline Vec3 operator-(double k, const Vec3& v2) {
  return Vec3(k - v2.x, k - v2.y, k - v2.z);
}

inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
inline Vec3 operator*(const Vec3& v1, double k) {
  return Vec3(v1.x * k, v1.y * k, v1.z * k);
}
inline Vec3 operator*(double k, const Vec3& v2) {
  return Vec3(k * v2.x, k * v2.y, k * v2.z);
}

inline Vec3 operator/(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}
inline Vec3 operator/(const Vec3& v1, double k) {
  return Vec3(v1.x / k, v1.y / k, v1.z / k);
}
inline Vec3 operator/(double k, const Vec3& v2) {
  return Vec3(k / v2.x, k / v2.y, k / v2.z);
}


inline Vec3 normalize(const Vec3& v) {
  return v/v.length();
}


inline double dot(const Vec3& v1, const Vec3& v2) {
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}


std::ostream& operator<<(std::ostream& stream, const Vec3& v) {
  stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return stream;
}


Vec3 pow(const Vec3& v, double p) {
  return Vec3(std::pow(v.x, p), std::pow(v.y, p), std::pow(v.z, p));
}


void orthonormalBasis(const Vec3& n, Vec3& vx, Vec3& vz) {
  if(n.x > 0.9) vx = Vec3(0, 1, 0);
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
#endif
