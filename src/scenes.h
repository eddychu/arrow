#pragma once

#include "hittable.h"
#include "integrator.h"
#include "material.h"
#include "renderer.h"
#include "sampler.h"
#include <memory>
#include <vector>

void scene1() {
  TestIntegrator integrator;
  Renderer renderer(512, 512, 100, 5, &integrator);
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
  Renderer renderer(width, height, 100, 5, &integrator);
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
  scene.build();
  renderer.render(scene, camera);
  renderer.save("scene2.png");
}

void scene3() {
  RandomSampler sampler;
  WhitIntegrator integrator;
  int width = 1024;
  int height = 768;
  Renderer renderer(width, height, 100, 5, &integrator);
  Camera camera(glm::vec3(0.0f, 1.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f), 45.0f,
                float(width) / float(height));
  auto material_ground =
      std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
  auto material_center =
      std::make_shared<Lambertian>(glm::vec3(0.1f, 0.2f, 0.5f));
  auto material_left = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f));
  auto material_right = std::make_shared<Dielectric>(1.5);

  auto material_light =
      std::make_shared<DiffuseLight>(glm::vec3(4.0f, 4.0f, 4.0f));

  std::vector<glm::vec3> vertices = {
      glm::vec3(-2.0f, 2.0f, -2.0f),
      glm::vec3(-2.0f, 2.0f, 2.0f),
      glm::vec3(2.0f, 2.0f, 2.0f),
      glm::vec3(2.0f, 2.0f, -2.0f),
  };

  std::vector<int> indices = {
      0, 1, 2, 0, 2, 3,
  };

  auto rect = std::make_unique<Mesh>(vertices, indices, material_light);

  Scene scene;
  scene.add(std::make_unique<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f,
                                     material_ground));
  scene.add(std::make_unique<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f,
                                     material_center));
  scene.add(std::make_unique<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f,
                                     material_left));
  scene.add(std::make_unique<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f,
                                     material_right));
  scene.add(std::make_unique<Mesh>(vertices, indices, material_light));
  scene.build();
  renderer.render(scene, camera);
  renderer.save("scene3.png");
}

void scene4() {
  RandomSampler sampler;
  WhitIntegrator integrator;
  int width = 600;
  int height = 600;
  Renderer renderer(width, height, 200, 5, &integrator);
  Camera camera(glm::vec3(26.0f, 3.0f, 6.0f), glm::vec3(0.0f, 2.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f), 20.0f,
                float(width) / float(height));
  Scene scene;
  scene.add(std::make_unique<Sphere>(
      glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f,
      std::make_shared<Lambertian>(glm::vec3(0.5f, 0.5f, 0.5f))));
  scene.add(std::make_unique<Sphere>(
      glm::vec3(0.0f, 2.0f, 0.0f), 2.0f,
      std::make_shared<Lambertian>(glm::vec3(0.5f, 0.5f, 0.5f))));
  scene.add(std::make_unique<Mesh>(
      std::vector<glm::vec3>({
          glm::vec3(3.0f, 1.0f, -2.0f),
          glm::vec3(5.0f, 1.0f, -2.0f),
          glm::vec3(5.0f, 3.0f, -2.0f),
          glm::vec3(3.0f, 3.0f, -2.0f),

      }),
      std::vector<int>({0, 1, 2, 0, 2, 3}),
      std::make_shared<DiffuseLight>(glm::vec3(4.0f, 4.0f, 4.0f))));
  scene.build();
  renderer.render(scene, camera);
  renderer.save("scene4.png");
}

/// cornell box
void scene5() {
  RandomSampler sampler;
  PathIntegrator integrator;
  int width = 600;
  int height = 600;
  Renderer renderer(width, height, 1000, 10, &integrator);
  Camera camera(glm::vec3(278.0f, 278.0f, -800.0f),
                glm::vec3(278.0f, 278.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                40.0f, float(width) / float(height));
  auto red = std::make_shared<Lambertian>(glm::vec3(0.65f, 0.05f, 0.05f));
  auto white = std::make_shared<Lambertian>(glm::vec3(0.73f, 0.73f, 0.73f));
  auto green = std::make_shared<Lambertian>(glm::vec3(0.12f, 0.45f, 0.15f));
  auto light = std::make_shared<DiffuseLight>(glm::vec3(15.0f, 15.0f, 15.0f));

  auto light_mesh =
      std::make_unique<Mesh>(std::vector<glm::vec3>({
                                 glm::vec3(343.0f, 548.8f, 227.0f),
                                 glm::vec3(343.0f, 548.8f, 332.0f),
                                 glm::vec3(213.0f, 548.8f, 332.0f),
                                 glm::vec3(213.0f, 548.8f, 227.0f),
                             }),
                             std::vector<int>({2, 1, 0, 3, 2, 0}), light);

  auto left_wall =
      std::make_unique<Mesh>(std::vector<glm::vec3>({
                                 glm::vec3(552.8f, 0.0f, 0.0f),
                                 glm::vec3(549.6f, 0.0f, 559.2f),
                                 glm::vec3(556.0, 548.8f, 559.2),
                                 glm::vec3(556.0, 548.8, 0.0),
                             }),
                             std::vector<int>({0, 1, 2, 2, 3, 0}), green);

  auto right_wall =
      std::make_unique<Mesh>(std::vector<glm::vec3>({
                                 glm::vec3(0.0f, 0.0f, 559.2f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 548.8f, 0.0f),
                                 glm::vec3(0.0f, 548.8f, 559.2f),
                             }),
                             std::vector<int>({0, 1, 2, 2, 3, 0}), red);

  auto floor =
      std::make_unique<Mesh>(std::vector<glm::vec3>({
                                 glm::vec3(552.8f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 0.0f, 559.2f),
                                 glm::vec3(549.6f, 0.0f, 559.2f),
                             }),
                             std::vector<int>({0, 1, 2, 2, 3, 0}), white);

  auto ceiling =
      std::make_unique<Mesh>(std::vector<glm::vec3>({
                                 glm::vec3(556.0f, 549.0f, 0.0f),
                                 glm::vec3(556.0f, 549.0f, 559.2f),
                                 glm::vec3(0.0f, 549.0f, 559.2f),
                                 glm::vec3(0.0f, 549.0f, 0.0f),
                             }),
                             std::vector<int>({0, 1, 2, 2, 3, 0}), white);

  auto back_wall =
      std::make_unique<Mesh>(std::vector<glm::vec3>({
                                 glm::vec3(556.0, 0.0f, 559.2f),
                                 glm::vec3(0.0f, 0.0f, 559.2f),
                                 glm::vec3(0.0f, 548.8f, 559.2f),
                                 glm::vec3(556.0f, 548.8f, 559.2f),
                             }),
                             std::vector<int>({0, 1, 2, 2, 3, 0}), white);

  auto short_block = std::make_unique<Mesh>(
      std::vector<glm::vec3>(
          {glm::vec3(130.0, 165.0, 65.0),  glm::vec3(82.0, 165.0, 225.0),
           glm::vec3(240.0, 165.0, 272.0), glm::vec3(290.0, 165.0, 114.0),
           glm::vec3(290.0, 0.0, 114.0),   glm::vec3(290.0, 165.0, 114.0),
           glm::vec3(240.0, 165.0, 272.0), glm::vec3(240.0, 0.0, 272.0),
           glm::vec3(130.0, 0.0, 65.0),    glm::vec3(130.0, 165.0, 65.0),
           glm::vec3(290.0, 165.0, 114.0), glm::vec3(290.0, 0.0, 114.0),
           glm::vec3(82.0, 0.0, 225.0),    glm::vec3(82.0, 165.0, 225.0),
           glm::vec3(130.0, 165.0, 65.0),  glm::vec3(130.0, 0.0, 65.0),
           glm::vec3(240.0, 0.0, 272.0),   glm::vec3(240.0, 165.0, 272.0),
           glm::vec3(82.0, 165.0, 225.0),  glm::vec3(82.0, 0.0, 225.0)}),
      std::vector<int>({3,  2,  0,  2,  1,  0,  7,  6,  4,  6,
                        5,  4,  11, 10, 8,  10, 9,  8,  15, 14,
                        12, 14, 13, 12, 19, 18, 16, 18, 17, 16}),
      white);

  auto tall_block = std::make_unique<Mesh>(
      std::vector<glm::vec3>({
          glm::vec3(423.0f, 330.0, 247.0),   glm::vec3(265.0f, 330.0f, 296.0f),
          glm::vec3(314.0f, 330.0f, 456.0f), glm::vec3(472.0f, 330.0f, 406.0f),
          glm::vec3(423.0f, 0.0f, 247.0f),   glm::vec3(423.0f, 330.0f, 247.0f),
          glm::vec3(472.0f, 330.0f, 406.0f), glm::vec3(472.0f, 0.0f, 406.0f),
          glm::vec3(472.0f, 0.0f, 406.0f),   glm::vec3(472.0f, 330.0f, 406.0f),
          glm::vec3(314.0f, 330.0f, 456.0f), glm::vec3(314.0f, 0.0f, 456.0f),
          glm::vec3(314.0f, 0.0f, 456.0f),   glm::vec3(314.0f, 330.0f, 456.0f),
          glm::vec3(265.0f, 330.0f, 296.0f), glm::vec3(265.0f, 0.0f, 296.0f),
          glm::vec3(265.0f, 0.0f, 296.0f),   glm::vec3(265.0f, 330.0f, 296.0f),
          glm::vec3(423.0f, 330.0f, 247.0f), glm::vec3(423.0f, 0.0f, 247.0f),

      }),
      std::vector<int>({3,  2,  0,  2,  1,  0,  7,  6,  4,  6,
                        5,  4,  11, 10, 8,  10, 9,  8,  15, 14,
                        12, 14, 13, 12, 19, 18, 16, 18, 17, 16}),
      white);

  Scene scene;
  // scene.add(std::move(left_wall));
  //  scene.add(std::move(right_wall));
  // scene.add(std::move(back_wall));

  scene.add(std::move(ceiling));
  scene.add(std::move(light_mesh));
  scene.add(std::move(left_wall));
  scene.add(std::move(right_wall));
  scene.add(std::move(floor));
  scene.add(std::move(back_wall));
  scene.add(std::move(short_block));
  scene.add(std::move(tall_block));
  scene.build();
  renderer.render(scene, camera);
  renderer.save("scene523.png");
}