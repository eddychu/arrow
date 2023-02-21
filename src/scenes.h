#pragma once

#include "hittable.h"
#include "integrator.h"
#include "material.h"
#include "renderer.h"
#include "sampler.h"
#include <memory>

void scene1() {
  TestIntegrator integrator;
  Renderer renderer(512, 512, 1, 3, &integrator);
  Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, 1.0f);
  auto material_ground =
      std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
  auto material_center =
      std::make_shared<Lambertian>(glm::vec3(0.1f, 0.2f, 0.5f));
  auto material_left = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f));
  auto material_right = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f));

  Scene scene;
  scene.add(std::make_unique<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f,
                                     material_ground));
  scene.add(std::make_unique<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f,
                                     material_center));
  scene.add(std::make_unique<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f,
                                     material_left));
  scene.add(std::make_unique<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f,
                                     material_right));

  renderer.render(scene, camera);
  renderer.save("scene1.png");
}

void scene2() {
  RandomSampler sampler;
  TestIntegrator integrator;
  int width = 1024;
  int height = 768;
  Renderer renderer(width, height, 10, 5, &integrator);
  Camera camera(glm::vec3(13.0f, 2.5f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f), 30.0f,
                float(width) / float(height));
  Scene scene;
  auto material_ground =
      std::make_shared<Lambertian>(glm::vec3(0.5f, 0.5f, 0.5f));
  scene.add(std::make_unique<Sphere>(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f,
                                     material_ground));
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      float choose_mat = sampler.get_1d();
      glm::vec3 center(a + 0.9f * sampler.get_1d(), 0.2f,
                       b + 0.9f * sampler.get_1d());
      if (glm::length(center - glm::vec3(4.0f, 0.2f, 0.0f)) > 0.9f) {
        std::shared_ptr<Material> sphere_material;
        if (choose_mat < 0.8f) {
          // diffuse
          auto albedo = glm::vec3(sampler.get_1d() * sampler.get_1d(),
                                  sampler.get_1d() * sampler.get_1d(),
                                  sampler.get_1d() * sampler.get_1d());
          sphere_material = std::make_shared<Lambertian>(albedo);
          scene.add(std::make_unique<Sphere>(center, 0.2f, sphere_material));
        } else if (choose_mat < 0.95f) {
          // metal
          auto albedo = glm::vec3(0.5f * (1 + sampler.get_1d()),
                                  0.5f * (1 + sampler.get_1d()),
                                  0.5f * (1 + sampler.get_1d()));
          auto fuzz = 0.5f * sampler.get_1d();
          sphere_material = std::make_shared<Metal>(albedo, fuzz);
          scene.add(std::make_unique<Sphere>(center, 0.2f, sphere_material));
        } else {
          // glass
          sphere_material = std::make_shared<Dielectric>(1.5f);
          scene.add(std::make_unique<Sphere>(center, 0.2f, sphere_material));
        }
      }
    }
  }

  auto material1 = std::make_shared<Dielectric>(1.5f);
  scene.add(
      std::make_unique<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, material1));
  auto material2 = std::make_shared<Lambertian>(glm::vec3(0.4f, 0.2f, 0.1f));
  scene.add(
      std::make_unique<Sphere>(glm::vec3(-4.0f, 1.0f, 0.0f), 1.0f, material2));
  auto material3 = std::make_shared<Lambertian>(glm::vec3(0.7f, 0.6f, 0.5f));
  scene.add(
      std::make_unique<Sphere>(glm::vec3(4.0f, 1.0f, 0.0f), 1.0f, material3));
  scene.build_accel();
  renderer.render(scene, camera);
  renderer.save("scene2.png");
}
