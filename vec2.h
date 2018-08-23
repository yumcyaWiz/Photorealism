#ifndef VEC2_H
#define VEC2_H
#include <cmath>
class Vec2 {
  public:
    double x;
    double y;

    Vec2() { x = y = 0; };
    Vec2(double _x) : x(_x), y(_x) {};
    Vec2(double _x, double _y) : x(_x), y(_y) {};

    double length() const {
      return std::sqrt(x*x + y*y);
    };
    double length2() const {
      return x*x + y*y;
    };
};
inline Vec2 operator+(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x + v2.x, v1.y + v2.y);
}
inline Vec2 operator+(const Vec2& v1, double k) {
  return Vec2(v1.x + k, v1.y + k);
}
inline Vec2 operator+(double k, const Vec2& v2) {
  return Vec2(k + v2.x, k + v2.y);
}

inline Vec2 operator-(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x - v2.x, v1.y - v2.y);
}
inline Vec2 operator-(const Vec2& v1, double k) {
  return Vec2(v1.x - k, v1.y - k);
}
inline Vec2 operator-(double k, const Vec2& v2) {
  return Vec2(k - v2.x, k - v2.y);
}

inline Vec2 operator*(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x * v2.x, v1.y * v2.y);
}
inline Vec2 operator*(const Vec2& v1, double k) {
  return Vec2(v1.x * k, v1.y * k);
}
inline Vec2 operator*(double k, const Vec2& v2) {
  return Vec2(k * v2.x, k * v2.y);
}

inline Vec2 operator/(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x / v2.x, v1.y / v2.y);
}
inline Vec2 operator/(const Vec2& v1, double k) {
  return Vec2(v1.x / k, v1.y / k);
}
inline Vec2 operator/(double k, const Vec2& v2) {
  return Vec2(k / v2.x, k / v2.y);
}

double dot(const Vec2& v1, const Vec2& v2) {
  return v1.x*v2.x + v1.y*v2.y;
}
double cross(const Vec2& v1, const Vec2& v2) {
  return v1.y*v2.x - v1.x*v2.y;
}
#endif
