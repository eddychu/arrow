#pragma once
#include "camera.h"
#include "integrator.h"
#include "sampler.h"
#include <chrono>
#include <iostream>
#include <ostream>
#include <vector>
class Renderer {
public:
  Renderer(int width, int height, int spp, int depth, Integrator *integrator)
      : m_width(width), m_height(height), m_spp(spp), m_depth(depth),
        m_integrator(integrator) {
    m_buffer.resize(m_width * m_height);
  }

  void render(const Scene &scene, const Camera &camera) {

    std::chrono::high_resolution_clock::time_point t1 =
        std::chrono::high_resolution_clock::now();

// use OpenMP to parallelize the loop
#pragma omp parallel for
    for (int y = 0; y < m_height; y++) {

      std::cout << "Rendering (y = " << y << "/" << m_height << ") "
                << "\r" << std::flush;

      for (int x = 0; x < m_width; x++) {
        RandomSampler sampler;
        glm::vec3 color(0.0f);
        for (int s = 0; s < m_spp; s++) {
          float u = (x + sampler.get_1d()) / m_width * 2.0f - 1.0f;
          float v = 1.0f - (y + sampler.get_1d()) / m_height * 2.0f;
          Ray ray = camera.get_ray(u, v);
          color += m_integrator->li(ray, scene, &sampler, m_depth);
        }
        color /= m_spp;
        color = glm::pow(color, glm::vec3(1.0f / 2.2f));
        m_buffer[y * m_width + x] = color;
      }
    }

    std::chrono::high_resolution_clock::time_point t2 =
        std::chrono::high_resolution_clock::now();
    /// get duration in seconds

    auto duration =
        std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    std::cout << "Render time: " << duration << " seconds" << std::endl;
  }

  void save(const std::string &filename) const;

private:
  int m_width;
  int m_height;
  int m_spp;
  int m_depth;
  Integrator *m_integrator;
  std::vector<glm::vec3> m_buffer;
};
