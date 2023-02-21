#pragma once
#include "camera.h"
#include "integrator.h"
#include "sampler.h"
#include <vector>
class Renderer {
public:
  Renderer(int width, int height, int spp, int depth, Integrator *integrator)
      : m_width(width), m_height(height), m_spp(spp), m_depth(depth),
        m_integrator(integrator) {
    m_buffer.resize(m_width * m_height);
  }

  void render(const Scene &scene, const Camera &camera) {
    RandomSampler sampler;

    for (int y = 0; y < m_height; y++) {
      for (int x = 0; x < m_width; x++) {
        glm::vec3 color(0.0f);
        for (int s = 0; s < m_spp; s++) {
          float u = (x + sampler.get_1d()) / m_width * 2.0f - 1.0f;
          float v = 1.0f - (y + sampler.get_1d()) / m_height * 2.0f;
          Ray ray = camera.get_ray(u, v);
          color += m_integrator->li(ray, scene, m_depth);
        }
        color /= m_spp;
        color = glm::pow(color, glm::vec3(1.0f / 2.2f));
        m_buffer[y * m_width + x] = color;
      }
    }
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