#pragma once

#include "config.hpp"
#include "ray.hpp"
#include "util.hpp"
#include "vec3.hpp"

static constexpr Vec3 up{0, 1, 0};

struct Camera {
  Point3 center{0, 0, 0};
  Point3 look_at{0, 0, -1};

  constexpr Camera() { init(); }

  constexpr void init() {
    sample_scale = 1.0f / SAMPLE_COUNT;

    w = norm(center - look_at);
    u = norm(up.cross(w));
    v = norm(u.cross(w));

    auto focal_len{(look_at - center).len()};
    auto vp_height{2 * std::tan(deg_to_rad(FOV) / 2) * focal_len};
    auto vp_width{vp_height * ASPECT};

    auto vp_u{vp_width * u};
    auto vp_v{vp_height * v};

    delta_u = vp_u / WIDTH;
    delta_v = vp_v / HEIGHT;

    auto top_left{center - w * focal_len - vp_u / 2 - vp_v / 2};
    first_pixel = top_left + 0.5 * (delta_u + delta_v);
  }

  constexpr Ray get_ray(const Vec3 &offset, size_t i, size_t j) const {
    auto sample{first_pixel + ((i + offset.x()) * delta_u) +
                ((j + offset.y()) * delta_v)};
    auto dir{sample - center};
    return {center, dir};
  }

  constexpr void move_x(float x) { center.x() += x; }

  constexpr void move_y(float y) { center.y() += y; }

  constexpr void move_z(float z) { center.z() += z; }

  constexpr void move(float x, float y, float z) {
    this->move_x(x);
    this->move_y(y);
    this->move_z(z);
  }

  constexpr void move(const Vec3 &v) { this->move(v.x(), v.y(), v.z()); }

private:
  Point3 first_pixel;
  Vec3 u, v, w;
  Vec3 delta_u, delta_v;
  float sample_scale;
};
