#pragma once

#include "util.hpp"
#include <cmath>

struct Vec3 {
  float v[3];

  constexpr Vec3() : v{0, 0, 0} {}

  constexpr Vec3(float x, float y, float z) : v{x, y, z} {}

  constexpr float &x() { return v[0]; }

  constexpr float &y() { return v[1]; }

  constexpr float &z() { return v[2]; }

  constexpr float x() const { return v[0]; }

  constexpr float y() const { return v[1]; }

  constexpr float z() const { return v[2]; }

  constexpr Vec3 operator-() const { return {-v[0], -v[1], -v[2]}; }

  constexpr float operator[](size_t i) const { return v[i]; }

  constexpr float &operator[](size_t i) { return v[i]; }

  constexpr Vec3 &operator+=(const Vec3 &o) {
    v[0] += o.v[0];
    v[1] += o.v[1];
    v[2] += o.v[2];
    return *this;
  }

  constexpr Vec3 &operator-=(const Vec3 &o) {
    v[0] -= o.v[0];
    v[1] -= o.v[1];
    v[2] -= o.v[2];
    return *this;
  }

  constexpr Vec3 &operator*=(float t) {
    v[0] *= t;
    v[1] *= t;
    v[2] *= t;
    return *this;
  }

  constexpr Vec3 &operator/=(float t) {
    v[0] /= t;
    v[1] /= t;
    v[2] /= t;
    return *this;
  }

  constexpr Vec3 cross(const Vec3 &o) const {
    return {v[1] * o.v[2] - v[2] * o.v[1], v[2] * o.v[0] - v[0] * o.v[2],
            v[0] * o.v[1] - v[1] * o.v[0]};
  }

  constexpr float dot(const Vec3 &o) const {
    return v[0] * o.v[0] + v[1] * o.v[1] + v[2] * o.v[2];
  }

  constexpr float len() const { return std::sqrt(len_sq()); };

  constexpr float len_sq() const {
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
  }

  constexpr bool near_zero() const {
    constexpr auto E{1e-8f};
    return std::abs(v[0]) < E && std::abs(v[1]) < E && std::abs(v[2]) < E;
  }

  static Vec3 random() {
    return Vec3{random_float(), random_float(), random_float()};
  }

  static Vec3 random(float min, float max) {
    return Vec3{random_float(min, max), random_float(min, max),
                random_float(min, max)};
  }
};

using Point3 = Vec3;

inline constexpr Vec3 operator+(Vec3 u, const Vec3 &v) {
  u += v;
  return u;
}

inline constexpr Vec3 operator-(Vec3 u, const Vec3 &v) {
  u -= v;
  return u;
}

inline constexpr Vec3 operator*(Vec3 u, float t) {
  u *= t;
  return u;
}

inline constexpr Vec3 operator*(float t, Vec3 u) { return u * t; }

inline constexpr Vec3 operator*(const Vec3 &u, const Vec3 &v) {
  return {u.x() * v.x(), u.y() * v.y(), u.z() * v.z()};
}

inline constexpr Vec3 operator/(Vec3 u, float t) {
  u /= t;
  return u;
}

inline constexpr Vec3 cross(const Vec3 &u, const Vec3 &v) { return u.cross(v); }

inline constexpr float dot(const Vec3 &u, const Vec3 &v) { return u.dot(v); }

inline constexpr Vec3 norm(const Vec3 &v) { return v / v.len(); }

inline Vec3 random_norm() {
  static constexpr auto epsilon{1e-45f};
  while (true) {
    auto v{Vec3::random(-1.0, 1.0)};
    auto len_sq{v.len_sq()};
    if (len_sq <= 1.0 && len_sq > epsilon) {
      return v / sqrt(len_sq);
    }
  }
}

inline Vec3 random_on_hemisphere(const Vec3 &normal) {
  auto v{random_norm()};
  return (normal.dot(v) > 0.0) ? v : -v;
}

inline constexpr Vec3 reflect(const Vec3 &v, const Vec3 &normal) {
  return v - 2 * dot(v, normal) * normal;
}

inline constexpr Vec3 refract(const Vec3 &v, const Vec3 &normal,
                              float eta_ratio) {
  auto cos{std::min(dot(-v, normal), 1.0f)};
  auto perp{eta_ratio * (v + cos * normal)};
  auto parallel{-normal * std::sqrt(1.0f - perp.len_sq())};
  return perp + parallel;
}
