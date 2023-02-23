#pragma once
#include "ray.h"
#include "record.h"
#include "sampler.h"
#include "sampling.h"
#include <cstdlib>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
class Material {
public:
  virtual ~Material() {}
  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       glm::vec3 &attenuation, Ray &scattered, float& pdf) const = 0;
  virtual glm::vec3 emitted(const Ray &ray, const HitRecord &rec) const {
    return glm::vec3(0.0f);
  }

  virtual glm::vec3 emitted() const {
    return glm::vec3(0.0f);
  }

  virtual float scattering_pdf(const Ray& ray, const HitRecord& rec, const Ray& scattered) const {
    return 0.0f;
  }

};

class Lambertian : public Material {
public:
  Lambertian(const glm::vec3 &a) : albedo(a) {}
  virtual bool scatter(const Ray &ray, const HitRecord &rec,
                       glm::vec3 &attenuation, Ray &scattered, float& pdf) const override {

    // auto normal =
    //     glm::dot(ray.direction(), rec.normal) < 0 ? rec.normal : -rec.normal;
    RandomSampler sampler;
    auto r1 = sampler.get_1d();
    auto r2 = sampler.get_1d();
    auto sample_dir = sample_cosine_weighted_hemisphere(r1, r2);
    // create a new coordinate system based on the normal
    auto w = rec.normal;
    auto a =
        w.x > 0.9f ? glm::vec3(0.0, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
    auto v = glm::normalize(glm::cross(w, a));
    auto u = glm::cross(w, v);
    auto dir = u * sample_dir.x + v * sample_dir.y + w * sample_dir.z;
    dir = glm::normalize(dir);
    scattered = Ray(rec.p + rec.normal * 0.0001f, dir);
    attenuation = albedo;
    pdf = glm::dot(rec.normal, dir) / M_PI;
    return true;

    // return glm::dot(rec.normal, ray.direction()) < 0;
    // return glm::dot(rec.normal, scattered.direction()) > 0;
  }

  virtual float scattering_pdf(const Ray& ray, const HitRecord& rec, const Ray& scattered) const override {
    auto cosine = glm::dot(rec.normal, scattered.direction());
    return cosine < 0.0f ? 0.0f : cosine / M_PI;
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
                       glm::vec3 &attenuation, Ray &scattered, float& pdf) const override {

    glm::vec3 reflected =
        glm::reflect(glm::normalize(ray.direction()), rec.normal);
    scattered = Ray(rec.p + rec.normal * 0.0001f, glm::normalize(reflected));
    attenuation = albedo;
    pdf = 1.0f;
    return glm::dot(ray.direction(), rec.normal) < 0;
  }

  glm::vec3 albedo;
  float fuzz;
};

class Dielectric : public Material {

public:
  Dielectric(float ri) : ref_idx(ri) {}
  virtual bool scatter(const Ray &ray, const HitRecord &rec,
                       glm::vec3 &attenuation, Ray &scattered, float& pdf) const override {
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
    pdf = 1.0f;
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

class DiffuseLight : public Material {
public:
  DiffuseLight(const glm::vec3 &a) : emit(a) {}
  virtual bool scatter(const Ray &ray, const HitRecord &rec,
                       glm::vec3 &attenuation, Ray &scattered, float& pdf) const override {
    return false;
  }
  virtual glm::vec3 emitted(const Ray &ray,
                            const HitRecord &rec) const override {
    if (glm::dot(rec.normal, ray.direction()) < 0) {
      return emit;
    } else {
      return glm::vec3(0.0f);
    }
  }

  virtual glm::vec3 emitted() const override {
    return emit;
  }

  glm::vec3 emit;
};