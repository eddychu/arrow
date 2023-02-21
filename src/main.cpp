#include "hittable.h"
#include "integrator.h"
#include "material.h"
#include "renderer.h"
#include <memory>

int main() {
  TestIntegrator integrator;
  Renderer renderer(512, 512, 16, 10, &integrator);
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
  renderer.save("output.png");
  return 0;
}