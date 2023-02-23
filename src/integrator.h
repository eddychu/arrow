#pragma once
#include "accel.h"
#include "hittable.h"
#include "ray.h"
#include "sampler.h"
class Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene, Sampler* sampler, int depth) const = 0;
};

class TestIntegrator : public Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,Sampler* sampler,
                       int depth) const override;
};

class WhitIntegrator : public Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,Sampler* sampler,
                       int depth) const override;
};

class PathIntegrator : public Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,Sampler* sampler,
                       int depth) const override;
private:
  glm::vec3 sample_lights(const HitRecord &record,
                          const Scene &scene, Sampler* sampler,  float& pdf) const;
};

class NormalIntegrator : public Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,Sampler* sampler,
                       int depth) const override;
};

class VisibilityIntegrator : public Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,Sampler* sampler,
                       int depth) const override;
};
