#pragma once

#include "config.hpp"
#include <cmath>

struct Vec3 {
  float v[3];

  constexpr HOSTDEV Vec3() : v{0, 0, 0} {}

  constexpr HOSTDEV Vec3(float x, float y, float z) : v{x, y, z} {}

  constexpr HOSTDEV float &x() { return v[0]; }

  constexpr HOSTDEV float &y() { return v[1]; }

  constexpr HOSTDEV float &z() { return v[2]; }

  constexpr HOSTDEV float x() const { return v[0]; }

  constexpr HOSTDEV float y() const { return v[1]; }

  constexpr HOSTDEV float z() const { return v[2]; }

  constexpr HOSTDEV Vec3 operator-() const { return {-v[0], -v[1], -v[2]}; }

  constexpr HOSTDEV float operator[](size_t i) const { return v[i]; }

  constexpr HOSTDEV float &operator[](size_t i) { return v[i]; }

  constexpr HOSTDEV Vec3 &operator+=(Vec3 o) {
    v[0] += o.v[0];
    v[1] += o.v[1];
    v[2] += o.v[2];
    return *this;
  }

  constexpr HOSTDEV Vec3 &operator-=(Vec3 o) {
    v[0] -= o.v[0];
    v[1] -= o.v[1];
    v[2] -= o.v[2];
    return *this;
  }

  constexpr HOSTDEV Vec3 &operator*=(Vec3 o) {
    v[0] *= o.v[0];
    v[1] *= o.v[1];
    v[2] *= o.v[2];
    return *this;
  }

  constexpr HOSTDEV Vec3 &operator*=(float t) {
    v[0] *= t;
    v[1] *= t;
    v[2] *= t;
    return *this;
  }

  constexpr HOSTDEV Vec3 &operator/=(float t) {
    v[0] /= t;
    v[1] /= t;
    v[2] /= t;
    return *this;
  }

  constexpr HOSTDEV Vec3 cross(Vec3 o) const {
    return {v[1] * o.v[2] - v[2] * o.v[1], v[2] * o.v[0] - v[0] * o.v[2],
            v[0] * o.v[1] - v[1] * o.v[0]};
  }

  constexpr HOSTDEV float dot(Vec3 o) const {
    return v[0] * o.v[0] + v[1] * o.v[1] + v[2] * o.v[2];
  }

  constexpr HOSTDEV float len() const { return std::sqrt(len_sq()); };

  constexpr HOSTDEV float len_sq() const {
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
  }

  constexpr HOSTDEV bool near_zero() const {
    constexpr auto E{1e-8f};
    return std::abs(v[0]) < E && std::abs(v[1]) < E && std::abs(v[2]) < E;
  }
};

using Point3 = Vec3;

inline constexpr HOSTDEV Vec3 operator+(Vec3 u, Vec3 v) {
  u += v;
  return u;
}

inline constexpr HOSTDEV Vec3 operator-(Vec3 u, Vec3 v) {
  u -= v;
  return u;
}

inline constexpr HOSTDEV Vec3 operator*(Vec3 u, float t) {
  u *= t;
  return u;
}

inline constexpr HOSTDEV Vec3 operator*(float t, Vec3 u) { return u * t; }

inline constexpr HOSTDEV Vec3 operator*(Vec3 u, Vec3 v) {
  return {u.x() * v.x(), u.y() * v.y(), u.z() * v.z()};
}

inline constexpr HOSTDEV Vec3 operator/(Vec3 u, float t) {
  u /= t;
  return u;
}

inline constexpr HOSTDEV Vec3 cross(Vec3 u, Vec3 v) { return u.cross(v); }

inline constexpr HOSTDEV float dot(Vec3 u, Vec3 v) { return u.dot(v); }

inline constexpr HOSTDEV Vec3 norm(Vec3 v) { return v / v.len(); }

inline constexpr HOSTDEV Vec3 reflect(Vec3 v, Vec3 normal) {
  return v - 2 * dot(v, normal) * normal;
}

inline constexpr HOSTDEV Vec3 refract(Vec3 v, Vec3 normal, float eta_ratio) {
  auto cos{std::min(dot(-v, normal), 1.0f)};
  auto perp{eta_ratio * (v + cos * normal)};
  auto parallel{-normal * std::sqrt(1.0f - perp.len_sq())};
  return perp + parallel;
}
