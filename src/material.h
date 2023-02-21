#pragma once
#include "ray.h"
#include "record.h"
#include "sampling.h"
#include <cstdlib>
#include <glm/geometric.hpp>
class Material {
public:
  virtual ~Material() {}
  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       glm::vec3 &attenuation, Ray &scattered) const = 0;
};

class Lambertian : public Material {
public:
  Lambertian(const glm::vec3 &a) : albedo(a) {}
  virtual bool scatter(const Ray &ray, const HitRecord &rec,
                       glm::vec3 &attenuation, Ray &scattered) const override {
    float r1 = rand() / (RAND_MAX + 1.0);
    float r2 = rand() / (RAND_MAX + 1.0);

    auto target = rec.p + rec.normal + sample_unit_sphere(r1, r2);
    scattered = Ray(rec.p, glm::normalize(target - rec.p));
    attenuation = albedo;
    return true;
  }

  glm::vec3 albedo;
};

class Metal : public Material {
public:
  Metal(const glm::vec3 &a, float f = 0.0f) : albedo(a) {
    if (f < 1) {
      fuzz = f;
    } else {
      fuzz = 1;
    }
  }
  virtual bool scatter(const Ray &ray, const HitRecord &rec,
                       glm::vec3 &attenuation, Ray &scattered) const override {
    glm::vec3 reflected =
        glm::reflect(glm::normalize(ray.direction()), rec.normal);
    float r1 = rand() / (RAND_MAX + 1.0);
    float r2 = rand() / (RAND_MAX + 1.0);
    scattered = Ray(rec.p, reflected + fuzz * sample_unit_sphere(r1, r2));
    attenuation = albedo;
    return (glm::dot(scattered.direction(), rec.normal) > 0);
  }

  glm::vec3 albedo;
  float fuzz;
};

class Dielectric : public Material {

public:
  Dielectric(float ri) : ref_idx(ri) {}
  virtual bool scatter(const Ray &ray, const HitRecord &rec,
                       glm::vec3 &attenuation, Ray &scattered) const override {
    attenuation = glm::vec3(1.0, 1.0, 1.0);
    bool is_front = dot(ray.direction(), rec.normal) < 0;
    auto normal = is_front ? rec.normal : -rec.normal;
    float etai_over_etat = is_front ? 1.0 / ref_idx : ref_idx;
    float cos_theta = glm::min(dot(-ray.direction(), normal), 1.0f);
    glm::vec3 refract_dir =
        glm::refract(ray.direction(), normal, etai_over_etat);
    float reflect_prob = schlick(cos_theta, etai_over_etat);
    float r1 = rand() / (RAND_MAX + 1.0);
    if (refract_dir == glm::vec3(0.0) || r1 < reflect_prob) {
      // only reflect
      glm::vec3 reflect_dir = glm::reflect(ray.direction(), normal);
      scattered = Ray(rec.p, normalize(reflect_dir));
    } else {
      // only refract
      scattered = Ray(rec.p, normalize(refract_dir));
    }
    return true;
  }

private:
  float ref_idx;

  static float schlick(float cosine, float ref_idx) {
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }
};