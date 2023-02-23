

#include "integrator.h"

#include "hittable.h"
#include "record.h"
#include "sampling.h"

glm::vec3 TestIntegrator::li(const Ray &ray, const Scene &scene,Sampler* sampler,
                             int depth) const {
  if (depth < 0) {
    return glm::vec3(0.0f);
  }
  HitRecord rec;
  if (scene.hit(ray, rec)) {

    // return (rec.normal + glm::vec3(1.0f, 1.0f, 1.0f)) * 0.5f;
    const auto &object = scene.get(rec.object_id.value());
    glm::vec3 attenuation;
    Ray scattered;
    float ignore;
    if (object->material()->scatter(ray, rec, attenuation, scattered, ignore)) {
      return attenuation * li(scattered, scene,sampler, depth - 1);
    }
    return attenuation;
  }

  glm::vec3 unit_direction = glm::normalize(ray.direction());
  float t = 0.5f * (unit_direction.y + 1.0f);
  return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) +
         t * glm::vec3(0.5f, 0.7f, 1.0f);
}

glm::vec3 WhitIntegrator::li(const Ray &ray, const Scene &scene,Sampler* sampler,
                             int depth) const {
  if (depth < 0) {
    return glm::vec3(0.0f);
  }
  HitRecord rec;
  if (!scene.hit(ray, rec)) {
    return glm::vec3(0.0f);
  }
  const auto &object = scene.get(rec.object_id.value());
  glm::vec3 attenuation;
  Ray scattered;
  float ignore;
  glm::vec3 emitted = object->material()->emitted(ray, rec);

  if (object->material()->scatter(ray, rec, attenuation, scattered, ignore)) {
    return emitted + attenuation * li(scattered, scene, sampler, depth - 1);
  }

  return emitted;
}

glm::vec3 PathIntegrator::li(const Ray &ray, const Scene &scene, Sampler* sampler, 
                             int depth) const {
    if (depth <= 0) {
      return glm::vec3(0.0, 0.0, 0.0);
    }
    HitRecord record;
    if (!scene.hit(ray, record)) {
      return glm::vec3(0.0, 0.0, 0.0);
    }
    const auto& object = scene.get(record.object_id.value());
    const auto& material = object->material();
    Ray scattered;
    glm::vec3 attenuation;

    glm::vec3 emitted = material->emitted(ray, record);
    float pdf;
    if (!material->scatter(ray, record, attenuation, scattered, pdf)) {
      return emitted;
    }

    auto light_dir = sample_lights(ray, record, scene, sampler, pdf);
    scattered = Ray(record.p, light_dir);
    return emitted + attenuation * material->scattering_pdf(ray, record, scattered) * li(scattered, scene, sampler, depth - 1) / pdf;
  }

glm::vec3 PathIntegrator::sample_lights(const Ray &ray, const HitRecord &record,
                          const Scene &scene, Sampler* sampler, float& pdf) const {
                            auto light_index = sampler->get_1d() * scene.lights.size();
                            auto light_id = scene.lights[light_index];
                            const auto& light = scene.get(light_id);
                            auto light_dir = light->sample(record, sampler);
                            auto light_pdf = light->pdf(record, light_dir);
                            pdf = light_pdf / scene.lights.size();
                            return light_dir;
                          }

glm::vec3 NormalIntegrator::li(const Ray &ray, const Scene &scene,Sampler* sampler,
                               int depth) const {
  HitRecord rec;
  if (!scene.hit(ray, rec)) {
    return glm::vec3(0.0f);
  }
  return (rec.normal + glm::vec3(1.0f, 1.0f, 1.0f)) * 0.5f;
}

glm::vec3 VisibilityIntegrator::li(const Ray &ray, const Scene &scene,Sampler* sampler,
                                   int depth) const {
  HitRecord rec;
  if (scene.hit(ray, rec)) {
    if (glm::dot(ray.direction(), rec.normal) < 0.0f) {
      return glm::vec3(1.0f);
    }
    return glm::vec3(0.0f);
  }
  return glm::vec3(0.0f);
}