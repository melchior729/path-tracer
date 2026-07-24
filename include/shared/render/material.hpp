#pragma once

#include "shared/render/background.hpp"
#include "shared/render/hit_record.hpp"

enum struct MaterialType { Lambertian, Metal, Dielectric };

struct Lambertian {
  Vec3 albedo;

  constexpr HOSTDEV void scatter(const HitRecord &record, Vec3 rand_norm,
                                 Vec3 &attenuation, Ray &out) const {
    auto dir{record.normal + rand_norm};
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

  bool HOSTDEV scatter(const Ray in, const HitRecord &record, Vec3 rand_norm,
                       Vec3 &attenuation, Ray &out) const {
    auto dir{reflect(in.direction(), record.normal)};
    dir = norm(dir) + fuzz * rand_norm;
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

  bool HOSTDEV scatter(const Ray in, const HitRecord record, float rand_float,
                       Vec3 &attenuation, Ray &out) const {
    attenuation = WHITE;
    auto ratio{record.front_facing ? 1.0 / refraction_idx : refraction_idx};
    auto dir{norm(in.direction())};

    auto cos{std::min(dot(-dir, record.normal), 1.0f)};
    auto sin{std::sqrt(1.0 - cos * cos)};

    if (ratio * sin > 1.0 || reflectance(cos, refraction_idx) > rand_float) {
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

    // pow is too slow
    return r0 + (1 - r0) *
                    ((1 - cos) * (1 - cos) * (1 - cos) * (1 - cos) * (1 - cos));
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
