#pragma once
#include "accel.h"
#include "hittable.h"
#include "ray.h"
class Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene, int depth) const = 0;
};

class TestIntegrator : public Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,
                       int depth) const override;
};

class WhitIntegrator : public Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,
                       int depth) const override;
};

class PathIntegrator : public Integrator {
public:
  PathIntegrator(int max_depth) : m_max_depth(max_depth) {}
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,
                       int depth) const override;

private:
  int m_max_depth;
};

class NormalIntegrator : public Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,
                       int depth) const override;
};

class VisibilityIntegrator : public Integrator {
public:
  virtual glm::vec3 li(const Ray &ray, const Scene &scene,
                       int depth) const override;
};
