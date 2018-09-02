#ifndef VEC2_H
#define VEC2_H
#include <iostream>
#include <cmath>
class Vec2 {
  public:
    float x;
    float y;

    Vec2() { x = y = 0; };
    Vec2(float _x) : x(_x), y(_x) {};
    Vec2(float _x, float _y) : x(_x), y(_y) {};

    float length() const {
      return std::sqrt(x*x + y*y);
    };
    float length2() const {
      return x*x + y*y;
    };
};
inline Vec2 operator+(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x + v2.x, v1.y + v2.y);
}
inline Vec2 operator+(const Vec2& v1, float k) {
  return Vec2(v1.x + k, v1.y + k);
}
inline Vec2 operator+(float k, const Vec2& v2) {
  return Vec2(k + v2.x, k + v2.y);
}

inline Vec2 operator-(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x - v2.x, v1.y - v2.y);
}
inline Vec2 operator-(const Vec2& v1, float k) {
  return Vec2(v1.x - k, v1.y - k);
}
inline Vec2 operator-(float k, const Vec2& v2) {
  return Vec2(k - v2.x, k - v2.y);
}

inline Vec2 operator*(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x * v2.x, v1.y * v2.y);
}
inline Vec2 operator*(const Vec2& v1, float k) {
  return Vec2(v1.x * k, v1.y * k);
}
inline Vec2 operator*(float k, const Vec2& v2) {
  return Vec2(k * v2.x, k * v2.y);
}

inline Vec2 operator/(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x / v2.x, v1.y / v2.y);
}
inline Vec2 operator/(const Vec2& v1, float k) {
  return Vec2(v1.x / k, v1.y / k);
}
inline Vec2 operator/(float k, const Vec2& v2) {
  return Vec2(k / v2.x, k / v2.y);
}

float dot(const Vec2& v1, const Vec2& v2) {
  return v1.x*v2.x + v1.y*v2.y;
}
float cross(const Vec2& v1, const Vec2& v2) {
  return v1.y*v2.x - v1.x*v2.y;
}

std::ostream& operator<<(std::ostream& stream, const Vec2& v) {
  stream << "(" << v.x << ", " << v.y << ")";
  return stream;
}
#endif
