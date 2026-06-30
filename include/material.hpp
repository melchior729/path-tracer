#pragma once

#include "constants.hpp"
#include "hit_record.hpp"
#include "ray.hpp"
#include "vec3.hpp"

enum struct MaterialType { Lambertian, Metal, Dielectric };

struct Lambertian {
  Vec3 albedo;

  void scatter(const HitRecord &record, Vec3 &attenuation, Ray &out) {
    auto dir{record.normal + random_norm()};
    if (dir.near_zero()) {
      dir = record.normal;
    }

    out = {record.p, dir};
    attenuation = albedo;
  };
};

struct Metal {
  Vec3 albedo;
  float fuzz;

  bool scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation,
               Ray &out) {
    auto dir{reflect(in.direction(), record.normal)};
    dir = norm(dir) + fuzz * random_norm();
    out = {record.p, dir};
    if (dot(dir, record.normal) > 0) {
      attenuation = albedo;
      return true;
    }

    return false;
  };
};

struct Dielectric {
  float refraction_idx;

  bool scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation,
               Ray &out) {
    attenuation = WHITE;
    auto ratio{record.front_facing ? 1.0 / refraction_idx : refraction_idx};
    auto dir{norm(in.direction())};

    auto cos{std::min(dot(-dir, record.normal), 1.0f)};
    auto sin{std::sqrt(1.0 - cos * cos)};

    if (ratio * sin > 1.0 ||
        reflectance(cos, refraction_idx) > random_float()) {
      dir = reflect(dir, record.normal);
    } else {
      dir = refract(dir, record.normal, ratio);
    }

    out = {record.p, dir};
    return true;
  };

  constexpr static float reflectance(float cos, float r_i) {
    auto r0{(1 - r_i) / (1 + r_i)};
    r0 *= r0;
    return r0 + (1 - r0) * std::pow(1 - cos, 5);
  }
};

struct Material {
  MaterialType type;
  union {
    Lambertian lambertian;
    Metal metal;
    Dielectric dielectric;
  };
};
