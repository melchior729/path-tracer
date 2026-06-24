#include "config.hpp"
#include "ray.hpp"
#include "util.hpp"
#include "vec3.hpp"

struct Camera {
  Point3 center{0, 0, 0};
  Point3 look_at{0, 0, -1};

private:
  static constexpr Vec3 up{0, 1, 0};
  Vec3 u, v, w;

  Point3 first_pixel;
  Vec3 delta_u, delta_v;
  float sample_scale;

  void init() {
    sample_scale = 1.0f / SAMPLE_COUNT;

    w = norm(center - look_at);
    u = norm(up.cross(w));
    v = norm(w.cross(u));

    auto focal_len{(look_at - center).len()};
    auto vp_height{2 * std::tan(deg_to_rad(FOV) / 2) * focal_len};
    auto vp_width{vp_height * ASPECT};

    auto vp_u{vp_width * u};
    auto vp_v{vp_width * v};

    delta_u = vp_u / WIDTH;
    delta_v = vp_v / HEIGHT;

    auto top_left{center - w * focal_len - vp_u / 2 - vp_v / 2};
    first_pixel = top_left + 0.5 * (delta_u + delta_v);
  }

  Ray get_ray(size_t i, size_t j) {
    auto offset{sample_square()};
    auto sample{first_pixel + ((i + offset.x()) * delta_u) +
                ((j + offset.y()) * delta_v)};
    auto dir{sample - center};
    return {center, dir};
  }

  Vec3 sample_square() const {
    return {random_float() - 0.5f, random_float() - 0.5f, 0};
  }
};
